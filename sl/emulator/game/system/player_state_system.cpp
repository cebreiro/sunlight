#include "player_state_system.h"

#include "sl/data/map/map_stage.h"
#include "sl/emulator/game/entity/game_npc.h"
#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/script/class/lua_player.h"

#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/player_group_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_npc_script_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/event_script/event_script.h"
#include "sl/emulator/game/contents/group/item_mix_prop_item_mapping.h"
#include "sl/emulator/game/contents/npc/npc_talk_box.h"
#include "sl/emulator/game/contents/skill/player_skill_target_selector.h"
#include "sl/emulator/game/contents/state/game_entity_state.h"
#include "sl/emulator/game/data/sox/item_etc.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/chat_message_creator.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/item_mix_message_creator.h"
#include "sl/emulator/game/message/creator/npc_message_creator.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/script/lua_script_engine.h"
#include "sl/emulator/game/script/class/lua_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/npc_shop_system.h"
#include "sl/emulator/game/system/player_group_system.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/player_quest_system.h"
#include "sl/emulator/game/system/player_skill_effect_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/zone_change_service.h"
#include "sl/emulator/service/gamedata/item/item_data.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    PlayerStateSystem::PlayerStateSystem(const ServiceLocator& serviceLocator, const MapStage& stageData)
        : _serviceLocator(serviceLocator)
        , _stageData(stageData)
    {
    }

    PlayerStateSystem::~PlayerStateSystem()
    {
    }

    void PlayerStateSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<ItemArchiveSystem>());
        Add(stage.Get<PlayerQuestSystem>());
        Add(stage.Get<NPCShopSystem>());
        Add(stage.Get<PlayerGroupSystem>());
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<PlayerIndexSystem>());
        Add(stage.Get<PlayerSkillEffectSystem>());
    }

    bool PlayerStateSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::PLAYER_SCRIPTSTATE,
            std::bind_front(&PlayerStateSystem::HandleScriptState, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::CHAR_STATE,
            std::bind_front(&PlayerStateSystem::HandleCharacterState, this)))
        {
            return false;
        }

        return true;
    }

    auto PlayerStateSystem::GetName() const -> std::string_view
    {
        return "player_state_system";
    }

    auto PlayerStateSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerStateSystem>();
    }

    auto PlayerStateSystem::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    void PlayerStateSystem::ShowEventScript(GamePlayer& player, const EventScript& eventScript)
    {
        player.Defer(GamePlayerMessageCreator::CreateEventScriptClear(player));

        for (const event_script_item_type& element : eventScript.GetItems())
        {
            std::visit([&]<typename T>(const T& item)
                {
                    if constexpr (std::is_same_v<T, EventScriptString>)
                    {
                        player.Defer(GamePlayerMessageCreator::CreateEventScriptAddString(player, item.tableIndex));
                    }
                    else if constexpr (std::is_same_v<T, EventScriptStringWithInt>)
                    {
                        player.Defer(GamePlayerMessageCreator::CreateEventScriptAddStringWithInt(player, item.tableIndex, item.value));
                    }
                    else if constexpr (std::is_same_v<T, EventScriptStringWithItem>)
                    {
                        player.Defer(GamePlayerMessageCreator::CreateEventScriptAddStringWithItem(player, item.tableIndex, item.itemId));
                    }
                    else
                    {
                        static_assert(sizeof(T), "not implemented");
                    }

                }, element);
        }

        player.Defer(GamePlayerMessageCreator::CreateEventScriptShow(player));
        player.FlushDeferred();
    }

    void PlayerStateSystem::CreateNPCTalkBox(GamePlayer& player, GameNPC& npc, const NPCTalkBox& talkBox)
    {
        player.Defer(NPCMessageCreator::CreateTalkBoxClear(npc));

        for (const npc_talk_box_item_type& element : talkBox.GetTalkBoxItems())
        {
            std::visit([&]<typename T>(const T & item)
            {
                if constexpr (std::is_same_v<T, NPCTalkBoxString>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddString(npc, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxStringWithInt>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddRuntimeIntString(npc, item.tableIndex, item.value));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxStringWithItem>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddItemName(npc, item.tableIndex, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxMenu>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddMenu(npc, item.tableIndexDefault, item.tableIndexMouseOver, item.index));
                }
                else
                {
                    static_assert(sizeof(T), "not implemented");
                }

            }, element);
        }

        player.Defer(NPCMessageCreator::CreateTalkBoxCreate(npc, player,
            talkBox.GetWidth(), talkBox.GetHeight()));
        player.FlushDeferred();
    }

    void PlayerStateSystem::DisposeNPCTalk(GamePlayer& player)
    {
        PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();
        if (!scriptComponent.HasTargetNPC())
        {
            return;
        }

        game_entity_id_type targetId = scriptComponent.GetTargetNPCId();
        player.Send(NPCMessageCreator::CreateTalkBoxClose(targetId));

        scriptComponent.Clear();
    }

    void PlayerStateSystem::ChangeStage(GamePlayer& player, int32_t stageId, int32_t destX, int32_t destY)
    {
        _serviceLocator.Get<ZoneChangeService>().StartStageChange(player, stageId, destX, destY);
    }

    void PlayerStateSystem::ChangeZone(GamePlayer& player, int32_t zoneId, int32_t destX, int32_t destY)
    {
        _serviceLocator.Get<ZoneChangeService>().StartZoneChange(player.GetClientId(), zoneId, destX, destY);
    }

    void PlayerStateSystem::OnUseItem(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const auto [targetItemId, targetItemType] = reader.ReadInt64();

        bool handled = false;

        do
        {
            const GameItem* targetItem = player.GetItemComponent().FindItem(game_entity_id_type(targetItemId));
            if (!targetItem || targetItem->GetType() != static_cast<GameEntityType>(targetItemType))
            {
                break;
            }

            const sox::ItemEtc* etcData = targetItem->GetData().GetEtcData();
            if (etcData&& etcData->isTool)
            {
                if (player.GetGroupComponent().HasGroup())
                {
                    break;
                }

                const auto failReason = [&]() -> std::optional<ItemMixOpenByItemFailReason>
                    {
                        if (!player.GetSkillComponent().FindSkill(etcData->skillID))
                        {
                            return ItemMixOpenByItemFailReason::NoItemMixSkill;
                        }

                        if (player.GetStateComponent().GetState().type == GameEntityStateType::Sitting)
                        {
                            return ItemMixOpenByItemFailReason::InvalidState;
                        }

                        if (!_stageData.terrain)
                        {
                            return ItemMixOpenByItemFailReason::NotTerrain;
                        }

                        return std::nullopt;
                    }();

                if (failReason.has_value())
                {
                    player.Send(ItemMixMessageCreator::CreateItemMixWindowByItemFail(player, *failReason));
                }
                else
                {
                    player.Send(ItemMixMessageCreator::CreateItemMixWindowByItemSuccess(player, *targetItem));
                }
            }
            else
            {
                break;
            }

            handled = true;
            
        } while (false);

        if (!handled)
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] unhandled item use. player: {}, target_item_id: {}",
                    GetName(), player.GetCId(), targetItemId));
        }
    }

    void PlayerStateSystem::HandleNPCConversation(GamePlayer& player, game_entity_id_type target)
    {
        do
        {
            const auto& entity = Get<SceneObjectSystem>().FindEntity(GameEntityType::NPC, target);
            if (!entity)
            {
                break;
            }

            GameNPC* npc = entity->Cast<GameNPC>();
            if (!npc)
            {
                assert(false);

                break;
            }

            if (const int32_t itemId = ItemMixPropItemMapping::FindItemId(npc->GetUnk1(), npc->GetUnk2());
                itemId != 0)
            {
                const ItemData* itemData = _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider().Find(itemId);
                if (!itemData)
                {
                    break;
                }

                const sox::ItemEtc* etcData = itemData->GetEtcData();
                if (!etcData)
                {
                    break;
                }

                if (!player.GetSkillComponent().FindSkill(etcData->skillID))
                {
                    player.Send(ItemMixMessageCreator::CreateItemMixWindowByPropFail(player));
                }
                else
                {
                    player.Send(ItemMixMessageCreator::CreateItemMixWindowByPropSuccess(player, itemId));
                }
            }
            else
            {
                PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();
                if (scriptComponent.HasTargetNPC())
                {
                    SUNLIGHT_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] player has already talk npc. player: {}, prev_npc: {}, new_npc: {}",
                            GetName(), player.GetCId(), scriptComponent.GetTargetNPCId(), target));

                    break;
                }

                scriptComponent.SetTargetNPCId(target);

                LuaSystem luaSystem(*this);
                LuaNPC luaNPC(*npc);
                LuaPlayer luaPlayer(*this, player);

                if (!_serviceLocator.Get<LuaScriptEngine>().ExecuteNPCScript(luaNPC.GetId(), luaSystem, luaNPC, luaPlayer, 0))
                {
                    scriptComponent.Clear();

                    break;
                }

                return;
            }
        }
        while (false);

        player.Send(NPCMessageCreator::CreateTalkBoxClose(target));
    }

    void PlayerStateSystem::HandlePlayerInteraction(GamePlayer& player, game_entity_id_type target)
    {
        const auto& entity = Get<SceneObjectSystem>().FindEntity(GameEntityType::Player, target);
        if (!entity)
        {
            return;
        }

        GamePlayer* targetPlayer = entity->Cast<GamePlayer>();
        assert(targetPlayer);

        const PlayerGroupComponent& groupComponent = targetPlayer->GetGroupComponent();
        if (!groupComponent.HasGroup() || groupComponent.GetGroupType() != GameGroupType::StreetVendor)
        {
            return;
        }

        Get<PlayerGroupSystem>().AddStreetVendorGuest(groupComponent.GetGroupId(), player);
    }

    void PlayerStateSystem::HandleScriptState(const ZoneMessage& message)
    {
        // selection == 0 -> exit state
        const int32_t selection = message.reader.Read<int32_t>();

        GamePlayer& player = message.player;
        PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();

        if (!scriptComponent.HasTargetNPC())
        {
            // npc script is disposed
            return;
        }

        do
        {
            const auto& entity = Get<SceneObjectSystem>().FindEntity(GameEntityType::NPC, scriptComponent.GetTargetNPCId());
            if (!entity)
            {
                break;
            }

            GameNPC* npc = entity->Cast<GameNPC>();
            if (!npc)
            {
                assert(false);

                break;
            }

            scriptComponent.SetSequence(scriptComponent.GetSequence() + 1);
            scriptComponent.SetSelection(selection);

            LuaSystem luaSystem(*this);
            LuaNPC luaNPC(*npc);
            LuaPlayer luaPlayer(*this, player);

            if (!_serviceLocator.Get<LuaScriptEngine>().ExecuteNPCScript(luaNPC.GetId(), luaSystem, luaNPC, luaPlayer, scriptComponent.GetSequence()))
            {
                scriptComponent.Clear();

                break;
            }

            return;

        } while (false);

        SUNLIGHT_LOG_WARN(_serviceLocator,
            fmt::format("[{}] fail to execute script. player: {}",
                GetName(), player.GetCId()));

        DisposeNPCTalk(player);
    }

    bool PlayerStateSystem::HandleCharacterState(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        BufferReader reader = message.reader.ReadObject();
        const GameEntityState state = GameEntityState::CreateFrom(reader);

        EntityStateComponent& stateComponent = player.GetStateComponent();
        const GameEntityStateType oldState = stateComponent.GetState().type;

        stateComponent.SetState(state);

        Get<EntityViewRangeSystem>().Broadcast(player,
            SceneObjectPacketCreator::CreateState(player), false);

        switch (state.type)
        {
        case GameEntityStateType::Idle:
        case GameEntityStateType::Moving:
            break;
        case GameEntityStateType::Disarming:
        case GameEntityStateType::Arming:
        {
            player.SetArmed(state.type == GameEntityStateType::Arming);
        }
        break;
        case GameEntityStateType::PickGroundItem:
        {
            HandlePickGroundItem(player, state.targetId);
        }
        break;
        case GameEntityStateType::Conversation:
        {
            HandleNPCConversation(player, state.targetId);
        }
        break;
        case GameEntityStateType::InteractWithPlayer:
        {
            HandlePlayerInteraction(player, state.targetId);
        }
        break;
        case GameEntityStateType::PlaySkill:
        {
            std::ostringstream oss;
            oss << "--------------------------------------------\n";
            oss << "type: " << ToString(state.type) << '\n';
            oss << "moveType: " << (int32_t)state.moveType << '\n';
            oss << "unk4: " << state.unk4 << '\n';
            oss << "unk5: " << state.unk5 << '\n';
            oss << "destX: " << state.destPosition.x() << '\n';
            oss << "destY: " << state.destPosition.y() << '\n';
            oss << "targetId: " << state.targetId.Unwrap() << '\n';
            oss << "targetType: " << ToString(state.targetType) << '\n';
            oss << "attackId: " << state.attackId << '\n';
            oss << "motionId: " << state.motionId << '\n';
            oss << "fxId: " << state.fxId << '\n';
            oss << "param1: " << state.param1 << '\n';
            oss << "param2: " << state.param2 << '\n';
            oss << "skillId: " << state.skillId << '\n';
            oss << "--------------------------------------------";

            SUNLIGHT_LOG_DEBUG(_serviceLocator, oss.str());

            HandlePlayerSkill(player, state);
        }
        break;
        case GameEntityStateType::None:
        case GameEntityStateType::NormalAttack:
        case GameEntityStateType::DamagedMotion:
        case GameEntityStateType::Dying:
        case GameEntityStateType::Dead:
        case GameEntityStateType::Entering:
        case GameEntityStateType::Leaving:
        case GameEntityStateType::DamageCancel:
        case GameEntityStateType::Greet:
        case GameEntityStateType::Resurrection:
        case GameEntityStateType::ChangingZone:
        case GameEntityStateType::PickStreetVendorItem:
        case GameEntityStateType::Hang:
        case GameEntityStateType::ChangingRoom:
        case GameEntityStateType::Sitting:
        case GameEntityStateType::StandingUp:
        case GameEntityStateType::UseItem:
        case GameEntityStateType::Emotion:
        case GameEntityStateType::UnkSkill:
        default:
            SUNLIGHT_LOG_DEBUG(_serviceLocator,
                fmt::format("[{}] unhandled player state. player: {}, state: {}",
                    GetName(), player.GetCId(), ToString(state.type)));
        }

        if (oldState != state.type)
        {
            Get<PlayerStatSystem>().OnStateChange(player, oldState, state.type);
        }

        return true;
    }

    void PlayerStateSystem::HandlePickGroundItem(GamePlayer& player, game_entity_id_type itemId)
    {
        SceneObjectSystem& sceneObjectSystem = Get<SceneObjectSystem>();

        const std::shared_ptr<GameEntity>& entity = sceneObjectSystem.FindEntity(GameEntityType::Item, itemId);
        if (!entity)
        {
            return;
        }

        GameItem* item = entity->Cast<GameItem>();
        assert(item);
        assert(item->GetQuantity() > 0);

        [[maybe_unused]]
        const int32_t quantity = item->GetQuantity();
        int32_t addQuantity = 0;

        const bool shouldRemove = Get<ItemArchiveSystem>().GainItem(player,
            std::static_pointer_cast<GameItem>(entity), addQuantity);

        if (addQuantity > 0)
        {
            const Eigen::Vector2f& position = entity->GetComponent<SceneObjectComponent>().GetPosition();

            player.Send(GamePlayerMessageCreator::CreatePlayerGainGroupItem(player,
                static_cast<int32_t>(position.x()), static_cast<int32_t>(position.y())));
        }

        if (shouldRemove)
        {
            sceneObjectSystem.RemoveItem(itemId);
        }
        else
        {
            assert(item->GetQuantity() > 0);
        }
    }

    void PlayerStateSystem::HandlePlayerSkill(GamePlayer& player, const GameEntityState& state)
    {
        Get<PlayerSkillEffectSystem>().OnSkillUse(player, state);
    }
}

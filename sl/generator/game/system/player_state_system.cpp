#include "player_state_system.h"

#include "sl/data/map/map_stage.h"

#include "sl/generator/game/component/entity_state_component.h"
#include "sl/generator/game/component/player_group_component.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/player_skill_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/event_script/event_script.h"
#include "sl/generator/game/contents/group/item_mix_prop_item_mapping.h"
#include "sl/generator/game/contents/state/game_entity_state.h"
#include "sl/generator/game/data/sox/item_etc.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/item_mix_message_creator.h"
#include "sl/generator/game/message/creator/npc_message_creator.h"
#include "sl/generator/game/message/creator/scene_object_message_creator.h"
#include "sl/generator/game/script/lua_script_engine.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/game/system/entity_skill_effect_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/game_script_system.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/zone_change_service.h"
#include "sl/generator/service/gamedata/item/item_data.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    PlayerStateSystem::PlayerStateSystem(const ServiceLocator& serviceLocator, const MapStage& stageData, int32_t zoneId)
        : _serviceLocator(serviceLocator)
        , _stageData(stageData)
        , _zoneId(zoneId)
    {
    }

    PlayerStateSystem::~PlayerStateSystem()
    {
    }

    void PlayerStateSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<ItemArchiveSystem>());
        Add(stage.Get<EntitySkillEffectSystem>());
        Add(stage.Get<PlayerGroupSystem>());
        Add(stage.Get<GameScriptSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
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
            GameEntity* entity = Get<SceneObjectSystem>().FindEntity(GameEntityType::NPC, target);
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
                if (!Get<GameScriptSystem>().StartNPCScript(player, *npc))
                {
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
        GameEntity* entity = Get<SceneObjectSystem>().FindEntity(GameEntityType::Player, target);
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

        Get<GameScriptSystem>().ContinueNPCScript(player, selection);
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
        case GameEntityStateType::Default:
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
        case GameEntityStateType::NormalAttack:
        {
            HandleNormalAttack(player, state);
        }
        break;
        case GameEntityStateType::Leaving:
        {
            // request revival
            if (state.motionId == -666 && player.IsDead())
            {
                int32_t destZoneId = 0;
                Eigen::Vector2f position;

                if (!GetRevivalPoint(_zoneId, destZoneId, position))
                {
                    destZoneId = _zoneId;
                    position = player.GetSceneObjectComponent().GetPosition();
                }

                player.SetDead(false);
                player.GetStatComponent().SetRecoveryStat(RecoveryStatType::HP, 1);

                _serviceLocator.Get<ZoneChangeService>().StartZoneChange(
                    player.GetClientId(), destZoneId, static_cast<int32_t>(position.x()), static_cast<int32_t>(position.y()));
            }
        }
        break;
        case GameEntityStateType::None:
        case GameEntityStateType::DamagedMotion:
        case GameEntityStateType::Dying:
        case GameEntityStateType::Dead:
        case GameEntityStateType::Entering:
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

        const std::shared_ptr<GameEntity>& entity = sceneObjectSystem.FindEntityShared(GameEntityType::Item, itemId);
        if (!entity)
        {
            return;
        }

        GameItem* item = entity->Cast<GameItem>();
        assert(item);
        assert(item->GetQuantity() > 0);

        bool shouldRemove = false;
        bool added = false;

        if (item->GetData().GetId() == GameConstant::ITEM_ID_GOLD)
        {
            Get<ItemArchiveSystem>().AddGold(player, item->GetQuantity());

            shouldRemove = true;
            added = true;
        }
        else
        {
            int32_t addQuantity = 0;

            shouldRemove = Get<ItemArchiveSystem>().GainItem(player,
                std::static_pointer_cast<GameItem>(entity), addQuantity);

            added = addQuantity > 0;

            if (!shouldRemove)
            {
                assert(item->GetQuantity() > 0);
            }
        }

        if (added)
        {
            const Eigen::Vector2f& position = entity->GetComponent<SceneObjectComponent>().GetPosition();

            player.Send(GamePlayerMessageCreator::CreatePlayerGainGroupItem(player,
                static_cast<int32_t>(position.x()), static_cast<int32_t>(position.y())));
        }

        if (shouldRemove)
        {
            sceneObjectSystem.RemoveItem(itemId);
        }
    }

    void PlayerStateSystem::HandlePlayerSkill(GamePlayer& player, const GameEntityState& state)
    {
        Get<EntitySkillEffectSystem>().OnSkillUse(player, state);
    }

    void PlayerStateSystem::HandleNormalAttack(GamePlayer& player, const GameEntityState& state)
    {
        Get<EntitySkillEffectSystem>().OnNormalAttackUse(player, state);
    }

    bool PlayerStateSystem::GetRevivalPoint(int32_t zoneId, int32_t& destZone, Eigen::Vector2f& destPos)
    {
        if (zoneId % 100 == 4)
        {
            destZone = 401;
            destPos = { 9650.f, 8850.f };

            return true;
        }

        switch (zoneId)
        {
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 107:
        case 108:
        case 109:
        case 110:
        case 111:
        case 112:
        case 113:
        case 114:
        case 305:
        case 311:
        case 303:
        case 306:
        {
            destZone = 201;
            destPos = { 9400.f, 8300.f };

            return true;
        }
        case 106:
        case 115:
        case 116:
        case 117:
        case 129:
        case 130:
        case 131:
        case 302:
        {
            destZone = 204;
            destPos = { 4500.f, 3100.f };

            return true;
        }
        case 118:
        case 119:
        case 120:
        case 308:
        case 304:
        {
            destZone = 202;
            destPos = { 10300.f, 3000.f };

            return true;
        }
        case 121:
        case 122:
        case 123:
        case 124:
        case 125:
        case 301:
        case 312:
        {
            destZone = 203;
            destPos = { 3500.f, 3500.f };

            return true;
        }

        case 126:
        case 127:
        case 128:
        {
            destZone = 205;
            destPos = { 1700.f, 3000.f };

            return true;
        }
        }

        return false;
    }
}

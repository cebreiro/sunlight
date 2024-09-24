#include "player_state_system.h"

#include "sl/emulator/game/script/class/lua_npc.h"
#include "sl/emulator/game/script/class/lua_player.h"
#include "sl/emulator/game/entity/game_npc.h"

#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/player_npc_script_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contants/npc/npc_talk_box.h"
#include "sl/emulator/game/contants/state/game_entity_state.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/npc_message_creator.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/script/lua_script_engine.h"
#include "sl/emulator/game/script/class/lua_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/player_quest_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    PlayerStateSystem::PlayerStateSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void PlayerStateSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<ItemArchiveSystem>());
        Add(stage.Get<PlayerQuestSystem>());
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

    void PlayerStateSystem::CreateNPCTalkBox(GamePlayer& player, GameNPC& npc, const NPCTalkBox& talkBox)
    {
        player.Defer(NPCMessageCreator::CreateTalkBoxClear(npc));

        for (const npc_talk_box_item_type& item : talkBox.GetTalkBoxItems())
        {
            std::visit([&]<typename T>(const T & item)
            {
                if constexpr (std::is_same_v<T, NPCTalkBoxContent>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddString(npc, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxContentWithInt>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddRuntimeIntString(npc, item.tableIndex, item.value));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxContentWithItem>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddItemName(npc, item.tableIndex, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxString>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddString(npc, item.tableIndex));
                }
                else if constexpr (std::is_same_v<T, NPCTalkBoxMenu>)
                {
                    player.Defer(NPCMessageCreator::CreateTalkBoxAddMenu(npc, item.tableIndexDefault, item.tableIndexMouseOver, item.index));
                }
                else
                {
                    static_assert(sizeof(T), "not implemented");
                }

            }, item);
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

    void PlayerStateSystem::StartNPCScript(GamePlayer& player, game_entity_id_type target)
    {
        do
        {
            PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();
            if (scriptComponent.HasTargetNPC())
            {
                SUNLIGHT_LOG_WARN(_serviceLocator,
                    fmt::format("[{}] player has already talk npc. player: {}, prev_npc: {}, new_npc: {}",
                        GetName(), player.GetCId(), scriptComponent.GetTargetNPCId(), target));

                break;
            }

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
        while (false);

        player.Send(NPCMessageCreator::CreateTalkBoxClose(target));
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
            StartNPCScript(player, state.targetId);
        }
        break;
        case GameEntityStateType::None:
        case GameEntityStateType::NormalAttack:
        case GameEntityStateType::PlaySkill:
        case GameEntityStateType::DamagedMotion:
        case GameEntityStateType::Dying:
        case GameEntityStateType::Dead:
        case GameEntityStateType::Entering:
        case GameEntityStateType::Leaving:
        case GameEntityStateType::DamageCancel:
        case GameEntityStateType::Greet:
        case GameEntityStateType::InteractWithPlayer:
        case GameEntityStateType::Resurrection:
        case GameEntityStateType::ChangingZone:
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
}

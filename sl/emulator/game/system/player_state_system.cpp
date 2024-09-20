#include "player_state_system.h"

#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contants/state/game_entity_state.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/item_archive_system.h"
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
    }

    bool PlayerStateSystem::Subscribe(Stage& stage)
    {
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
        case GameEntityStateType::None:
        case GameEntityStateType::NormalAttack:
        case GameEntityStateType::PlaySkill:
        case GameEntityStateType::DamagedMotion:
        case GameEntityStateType::Conversation:
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

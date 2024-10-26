#include "entity_movement_system.h"

#include "sl/emulator/game/component/entity_movement_component.h"
#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/movement/client_movement.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_request.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    void EntityMovementSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
    }

    bool EntityMovementSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZonePacketC2S::NMC_REQ_STOP,
            std::bind_front(&EntityMovementSystem::HandleMovement, this)))
        {
            return false;
        }

        return GameSystem::Subscribe(stage);
    }

    bool EntityMovementSystem::ShouldUpdate() const
    {
        return true;
    }

    void EntityMovementSystem::Update()
    {
        if (_movingEntities.empty())
        {
            return;
        }

        const game_time_point_type now = GameTimeService::Now();

        for (auto iter = _movingEntities.begin(); iter != _movingEntities.end(); )
        {
            GameEntity& entity = *iter->second;

            EntityMovementComponent& movementComponent = entity.GetComponent<EntityMovementComponent>();
            if (movementComponent.IsMoving())
            {
                if (ClientMovement* clientMovement = movementComponent.GetForwardMovement(); clientMovement)
                {
                    std::chrono::duration<float> duration = (now - movementComponent.GetStartTimePoint());
                    const float totalDistance = (clientMovement->destPosition - clientMovement->position).norm();
                    const float totalTime = totalDistance / clientMovement->speed / 100;

                    const float t = std::min(duration.count() / totalTime, 1.f);

                    const Eigen::Vector2f newPosition = ((1.f - t) * clientMovement->position) + (t * clientMovement->destPosition);

                    SceneObjectComponent& sceneObjectComponent = entity.GetComponent<SceneObjectComponent>();
                    sceneObjectComponent.SetPosition(newPosition);

                    Get<EntityViewRangeSystem>().UpdateViewRange(entity, newPosition);

                    if (t >= 1.f)
                    {
                        movementComponent.SetIsMoving(false);
                        sceneObjectComponent.SetMoving(false);

                        iter = _movingEntities.erase(iter);

                        continue;
                    }
                }
                else
                {
                    assert(false);
                }
            }

            ++iter;
        }
    }

    auto EntityMovementSystem::GetName() const -> std::string_view
    {
        return "entity_movement_system";
    }

    auto EntityMovementSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityMovementSystem>();
    }

    bool EntityMovementSystem::Remove(game_entity_id_type id)
    {
        return _movingEntities.erase(id);
    }

    void EntityMovementSystem::Teleport(GameEntity& entity, Eigen::Vector2f position)
    {
        SceneObjectComponent* sceneObjectComponent = entity.FindComponent<SceneObjectComponent>();
        if (!sceneObjectComponent)
        {
            assert(false);

            return;
        }

        sceneObjectComponent->SetMoving(false);
        sceneObjectComponent->SetPosition(position);
        sceneObjectComponent->SetDestPosition(position);

        if (GamePlayer* player = entity.Cast<GamePlayer>(); player)
        {
            player->Send(ZonePacketS2CCreator::CreateObjectForceMove(entity));
        }

        Remove(entity.GetId());

        Get<EntityViewRangeSystem>().UpdateViewRange(entity, sceneObjectComponent->GetPosition());
    }

    void EntityMovementSystem::MoveTo(GameEntity& entity, Eigen::Vector2f position, float speed)
    {
        if (speed <= 0.f)
        {
            return;
        }

        EntityStateComponent& stateComponent = entity.GetComponent<EntityStateComponent>();
        stateComponent.SetState(GameEntityState{
            .type = GameEntityStateType::Moving,
            .moveType = GameEntityState::MoveType::Walk,
            .destPosition = Eigen::Vector3f(position.x(), position.y(), 0.f),
            });

        EntityMovementComponent& movementComponent = entity.GetComponent<EntityMovementComponent>();
        SceneObjectComponent& sceneObjectComponent = entity.GetComponent<SceneObjectComponent>();

        ClientMovement movement;
        movement.position = sceneObjectComponent.GetPosition();
        movement.destPosition = position;
        movement.speed = speed;
        movement.yaw = CreateYaw(sceneObjectComponent.GetPosition(), position);
        movement.movementTypeBitMask = 0x10;

        movementComponent.SetStartTimePoint(GameTimeService::Now());
        movementComponent.SetClientMovement(movement);
        sceneObjectComponent.Set(movement);

        _movingEntities[entity.GetId()] = &entity;

        Get<EntityViewRangeSystem>().VisitPlayer(entity, [&entity](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(entity));
                player.Defer(SceneObjectPacketCreator::CreateState(entity));
                player.FlushDeferred();
            });
    }

    void EntityMovementSystem::HandleMovement(const ZoneRequest& request)
    {
        GamePlayer& player = request.player;

        BufferReader reader1 = request.reader.ReadObject();
        ClientMovement movement1 = ClientMovement::CreateFrom(reader1);
        (void)movement1;

        BufferReader reader2 = request.reader.ReadObject();
        ClientMovement movement2 = ClientMovement::CreateFrom(reader2);

        EntityMovementComponent& movementComponent = player.GetMovementComponent();
        movementComponent.SetStartTimePoint(GameTimeService::Now());
        movementComponent.SetClientMovement(movement2);

        SceneObjectComponent& sceneObjectComponent = player.GetSceneObjectComponent();
        sceneObjectComponent.Set(movement2);

        if (movementComponent.IsMoving())
        {
            _movingEntities[player.GetId()] = &player;
        }
        else
        {
            _movingEntities.erase(player.GetId());
        }

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();
        viewRangeSystem.Broadcast(player, ZonePacketS2CCreator::CreateObjectMove(player), false);
        viewRangeSystem.UpdateViewRange(player, sceneObjectComponent.GetPosition());
    }

    auto EntityMovementSystem::CreateYaw(const Eigen::Vector2f& from, const Eigen::Vector2f& to) -> float
    {
        const Eigen::Vector2f vector = (to - from);

        return std::atan2f(vector.y(), vector.x())* (180.f / static_cast<float>(std::numbers::pi));
    }
}

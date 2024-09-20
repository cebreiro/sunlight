#include "entity_movement_system.h"

#include "sl/emulator/game/component/entity_movement_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contants/movement/forward_movement.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_request.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"

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
                if (ForwardMovement* forwardMovement = movementComponent.GetForwardMovement(); forwardMovement)
                {
                    std::chrono::duration<float> duration = (now - movementComponent.GetStartTimePoint());
                    const float totalDistance = (forwardMovement->destPosition - forwardMovement->position).norm();
                    const float totalTime = totalDistance / forwardMovement->speed / 100;

                    const float t = std::min(duration.count() / totalTime, 1.f);

                    const Eigen::Vector2f newPosition = ((1.f - t) * forwardMovement->position) + (t * forwardMovement->destPosition);

                    entity.GetComponent<SceneObjectComponent>().SetPosition(newPosition);

                    Get<EntityViewRangeSystem>().UpdateViewRange(entity, newPosition);

                    if (t >= 1.f)
                    {
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

    void EntityMovementSystem::HandleMovement(const ZoneRequest& request)
    {
        GamePlayer& player = request.player;

        BufferReader reader1 = request.reader.ReadObject();
        ForwardMovement movement1 = ForwardMovement::CreateFrom(reader1);

        BufferReader reader2 = request.reader.ReadObject();
        ForwardMovement movement2 = ForwardMovement::CreateFrom(reader2);

        (void)movement1;

        EntityMovementComponent& movementComponent = player.GetMovementComponent();
        movementComponent.SetStartTimePoint(GameTimeService::Now());
        movementComponent.SetForwardMovement(movement2);

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

        Get<EntityViewRangeSystem>().UpdateViewRange(player, sceneObjectComponent.GetPosition());
    }
}

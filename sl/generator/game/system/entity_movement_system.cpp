#include "entity_movement_system.h"

#include "sl/generator/game/component/entity_movement_component.h"
#include "sl/generator/game/component/entity_observable_component.h"
#include "sl/generator/game/component/entity_state_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/movement/client_movement.h"
#include "sl/generator/game/contents/movement/path_point_movement.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_request.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/scene_object_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/player_index_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    void EntityMovementSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerIndexSystem>());

        if (PathFindingSystem* pathFindSystem = stage.Find<PathFindingSystem>();
            pathFindSystem)
        {
            Add(*pathFindSystem);
        }
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
            SceneObjectComponent& sceneObjectComponent = entity.GetComponent<SceneObjectComponent>();

            bool stopped = false;

            if (ClientMovement* clientMovement = movementComponent.GetClientMovement(); clientMovement)
            {
                std::chrono::duration<float> duration = (now - movementComponent.GetStartTimePoint());
                const float totalDistance = (clientMovement->destPosition - clientMovement->position).norm();
                const float totalTime = totalDistance / clientMovement->speed / 100.f;

                const float t = std::min(duration.count() / totalTime, 1.f);

                const Eigen::Vector2f newPosition = ((1.f - t) * clientMovement->position) + (t * clientMovement->destPosition);
                sceneObjectComponent.SetPosition(newPosition);

                Get<EntityViewRangeSystem>().UpdateViewRange(entity, newPosition);

                NotifyNewPosition(entity, newPosition);

                if (t >= 1.f)
                {
                    stopped = true;

                    movementComponent.Reset();
                    sceneObjectComponent.SetMoving(false);

                    if (entity.GetType() == GameEntityType::Enemy)
                    {
                        EntityStateComponent& stateComponent = entity.GetComponent<EntityStateComponent>();
                        stateComponent.SetState(GameEntityState{ .type = GameEntityStateType::Default, });

                        Get<EntityViewRangeSystem>().VisitPlayer(entity, [&entity](GamePlayer& visited)
                        {
                            visited.Send(ZonePacketS2CCreator::CreateObjectMove(entity));
                            visited.Send(SceneObjectPacketCreator::CreateState(entity));
                        });
                    }

                    iter = _movingEntities.erase(iter);

                    continue;
                }
            }
            else if (PathPointMovement* pathMovement = movementComponent.GetPathPointMovement(); pathMovement)
            {
                int64_t oldIndex = 0;
                const Eigen::Vector2f newPosition = pathMovement->CalculatePointOnPath(now, oldIndex);

                std::swap(pathMovement->pathIndex, oldIndex);

                sceneObjectComponent.SetPosition(newPosition);

                Get<EntityViewRangeSystem>().UpdateViewRange(entity, newPosition);

                NotifyNewPosition(entity, newPosition);

                if (oldIndex != pathMovement->pathIndex)
                {
                    if (const int64_t lastIndex = std::ssize(pathMovement->paths) - 1;
                        oldIndex != lastIndex)
                    {
                        const Eigen::Vector2f& destPosition = pathMovement->paths[std::min(pathMovement->pathIndex, lastIndex)].second;

                        sceneObjectComponent.SetYaw(CalculateYaw(newPosition, destPosition));
                        sceneObjectComponent.SetDestPosition(destPosition);

                        Get<EntityViewRangeSystem>().Broadcast(entity, ZonePacketS2CCreator::CreateObjectMove(entity), false);
                    }
                }

                if (pathMovement->pathIndex >= std::ssize(pathMovement->paths))
                {
                    stopped = true;
                }
            }
            else
            {
                assert(false);
            }

            if (stopped)
            {
                movementComponent.Reset();
                sceneObjectComponent.SetMoving(false);

                EntityStateComponent& stateComponent = entity.GetComponent<EntityStateComponent>();
                stateComponent.SetState(GameEntityState{ .type = GameEntityStateType::Default });

                Get<EntityViewRangeSystem>().Broadcast(entity, SceneObjectPacketCreator::CreateState(entity), false);

                iter = _movingEntities.erase(iter);
            }
            else
            {
                if (now <= movementComponent.GetLastSyncTimePoint() + std::chrono::milliseconds(100))
                {
                    Get<EntityViewRangeSystem>().Broadcast(entity, ZonePacketS2CCreator::CreateObjectMove(entity), false);

                    movementComponent.SetLastSyncTimePoint(now);
                }

                ++iter;
            }
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

        Remove(entity.GetId());

        sceneObjectComponent->SetMoving(false);
        sceneObjectComponent->SetPosition(position);
        sceneObjectComponent->SetDestPosition(position);

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        viewRangeSystem.Broadcast(entity, ZonePacketS2CCreator::CreateObjectForceMove(entity), true);
        viewRangeSystem.UpdateViewRange(entity, sceneObjectComponent->GetPosition());
    }

    void EntityMovementSystem::MoveToPosition(GameEntity& entity, Eigen::Vector2f position, float speed)
    {
        if (speed <= 0.f)
        {
            return;
        }

        /*EntityStateComponent& stateComponent = entity.GetComponent<EntityStateComponent>();
        stateComponent.SetState(GameEntityState{
            .type = GameEntityStateType::Moving,
            .moveType = GameEntityState::MoveType::Walk,
            .destPosition = Eigen::Vector3f(position.x(), position.y(), 0.f),
            });*/

        EntityMovementComponent& movementComponent = entity.GetComponent<EntityMovementComponent>();
        SceneObjectComponent& sceneObjectComponent = entity.GetComponent<SceneObjectComponent>();

        ClientMovement movement;
        movement.position = sceneObjectComponent.GetPosition();
        movement.destPosition = position;
        movement.speed = speed;
        movement.yaw = CalculateYaw(sceneObjectComponent.GetPosition(), position);
        movement.movementTypeBitMask = 0x10;
        movement.unk1 = 123.f;
        movement.unk2 = 0xAABB;

        movementComponent.SetStartTimePoint(GameTimeService::Now());
        movementComponent.SetLastSyncTimePoint(GameTimeService::Now());
        movementComponent.SetClientMovement(movement);
        sceneObjectComponent.Set(movement);

        _movingEntities[entity.GetId()] = &entity;

        Get<EntityViewRangeSystem>().VisitPlayer(entity, [&entity](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(entity));
                //player.Defer(SceneObjectPacketCreator::CreateState(entity));
                player.FlushDeferred();
            });
    }

    void EntityMovementSystem::MoveToTarget(GameEntity& entity, const GameEntity& target, float speed)
    {
        const Eigen::Vector2f targetPosition = target.GetComponent<SceneObjectComponent>().GetPosition();

        do
        {
            PathFindingSystem* pathFindSystem = Find<PathFindingSystem>();
            if (!pathFindSystem)
            {
                break;
            }

            std::vector<Eigen::Vector2f> paths;
            const Eigen::Vector2f entityPosition = entity.GetComponent<SceneObjectComponent>().GetPosition();

            if (!pathFindSystem->FindPath(paths, entityPosition, targetPosition))
            {
                break;
            }

            if (std::ssize(paths) < 2)
            {
                break;
            }

            paths.front() = entityPosition;
            paths.back() = targetPosition;

            const game_time_point_type now = GameTimeService::Now();

            PathPointMovement pathPointMovement;
            pathPointMovement.paths.reserve(paths.size());
            pathPointMovement.paths.emplace_back(now, paths[0]);
            pathPointMovement.pathIndex = 1;

            game_time_point_type prevEndTimePoint = now;

            for (int32_t i = 1; i < std::ssize(paths); ++i)
            {
                const auto duration = std::chrono::duration<float>((paths[i] - paths[i - 1]).norm() / speed / 100.f);
                const game_time_point_type currentEndTimePoint = prevEndTimePoint + std::chrono::duration_cast<std::chrono::milliseconds>(duration);

                pathPointMovement.paths.emplace_back(currentEndTimePoint,paths[i]);

                prevEndTimePoint = currentEndTimePoint;
            }

            EntityStateComponent& stateComponent = entity.GetComponent<EntityStateComponent>();
            stateComponent.SetState(GameEntityState{
                .type = GameEntityStateType::Moving,
                .moveType = GameEntityState::MoveType::Walk,
                .destPosition = Eigen::Vector3f(paths[1].x(), paths[1].y(), 0.f),
                });

            EntityMovementComponent& movementComponent = entity.GetComponent<EntityMovementComponent>();
            movementComponent.SetPathPointMovement(std::move(pathPointMovement));
            movementComponent.SetStartTimePoint(GameTimeService::Now());
            movementComponent.SetLastSyncTimePoint(GameTimeService::Now());

            SceneObjectComponent& sceneObjectComponent = entity.GetComponent<SceneObjectComponent>();

            ClientMovement movement;
            movement.position = sceneObjectComponent.GetPosition();
            movement.destPosition = paths[1];
            movement.speed = speed;
            movement.yaw = CalculateYaw(sceneObjectComponent.GetPosition(), paths[1]);
            movement.movementTypeBitMask = 0x10;

            sceneObjectComponent.Set(movement);

            _movingEntities[entity.GetId()] = &entity;

            Get<EntityViewRangeSystem>().VisitPlayer(entity, [&entity](GamePlayer& player)
                {
                    player.Defer(ZonePacketS2CCreator::CreateObjectMove(entity));
                    player.Defer(SceneObjectPacketCreator::CreateState(entity));
                    player.FlushDeferred();
                });

            return;
            
        } while (false);

        MoveToPosition(entity, targetPosition, speed);
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

    auto EntityMovementSystem::CalculateYaw(const Eigen::Vector2f& from, const Eigen::Vector2f& to) -> float
    {
        const Eigen::Vector2f vector = (to - from);

        return std::atan2f(vector.y(), vector.x()) * (180.f / static_cast<float>(std::numbers::pi));
    }

    void EntityMovementSystem::NotifyNewPosition(const GameEntity& entity, Eigen::Vector2f position)
    {
        const auto* observableComponent = entity.FindComponent<EntityObservableComponent>();

        if (!observableComponent || !observableComponent->HasObserver(ObservableType::EntityPosition))
        {
            return;
        }

        PlayerIndexSystem& sceneObjectSystem = Get<PlayerIndexSystem>();

        for (const int64_t playerId : observableComponent->GetObserverRange(ObservableType::EntityPosition))
        {
            GamePlayer* observer = sceneObjectSystem.FindByCId(playerId);
            if (!observer)
            {
                continue;
            }

            const int32_t x = static_cast<int32_t>(position.x());
            const int32_t y = static_cast<int32_t>(position.y());

            observer->Send(GamePlayerMessageCreator::CreatePlayerGainGroupItem(*observer, x, y));
        }
    }
}

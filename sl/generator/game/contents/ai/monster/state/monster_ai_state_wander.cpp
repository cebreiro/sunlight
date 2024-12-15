#include "monster_ai_state_wander.h"

#include "sl/generator/game/component/monster_aggro_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/ai/monster/monster_controller.h"
#include "sl/generator/game/data/sox/monster_action.h"
#include "sl/generator/game/data/sox/monster_base.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/entity_scan_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    MonsterAIStateWander::MonsterAIStateWander()
        : IState(MonsterAIStateType::Wander)
    {
    }

    auto MonsterAIStateWander::OnEvent(const MonsterAIStateParam& event) -> Future<void>
    {
        EntityAIControlSystem& system = event.system;
        MonsterController& controller = event.controller;
        GameMonster& monster = event.monster;
        MonsterAggroComponent& monsterAggroComponent = monster.GetAggroComponent();

        const sox::MonsterAction& actionData = monster.GetData().GetAction();
        std::mt19937& randomEngine = controller.GetRandomEngine();

        if (actionData.characteristic == 1 && actionData.scanTime > 0)
        {
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(GameTimeService::Now() - _lastScanTime);
            if (duration >= std::chrono::milliseconds(actionData.scanTime))
            {
                _scanResult.clear();
                co_await system.Get<EntityScanSystem>().ScanPlayerAsync(_scanResult, monster, static_cast<float>(actionData.sightRange));

                if (controller.ShouldStopCoroutine())
                {
                    co_return;
                }

                controller.ConfigureCoroutineExecutionContext();

                _lastScanTime = GameTimeService::Now();

                if (!_scanResult.empty())
                {
                    std::optional<game_entity_id_type> combatTargetId = std::nullopt;

                    while (!_scanResult.empty())
                    {
                        const auto iter = std::ranges::min_element(_scanResult,
                            [](const std::pair<game_entity_id_type, float>& lhs, const std::pair<game_entity_id_type, float>& rhs) -> bool
                            {
                                return lhs.first < rhs.first;
                            });
                        assert(iter != _scanResult.end());

                        bool done = false;

                        do
                        {
                            const GameEntity* entity = system.Get<SceneObjectSystem>().FindEntity(GamePlayer::TYPE, iter->first);
                            if (!entity)
                            {
                                break;
                            }

                            if (entity->GetType() == GamePlayer::TYPE)
                            {
                                const GamePlayer* player = entity->Cast<GamePlayer>();
                                assert(player);

                                if (const int32_t levelDiff = player->GetStatComponent().GetLevel() - monster.GetData().GetBase().level;
                                    levelDiff > actionData.levelDiffMax)
                                {
                                    break;
                                }
                            }

                            done = true;
                            combatTargetId = entity->GetId();
                            
                        } while (false);

                        if (done)
                        {
                            break;
                        }

                        _scanResult.erase(iter);
                    }

                    if (combatTargetId.has_value())
                    {
                        monsterAggroComponent.AddValue(*combatTargetId, 1);
                    }
                }
            }
        }

        monsterAggroComponent.RemoveGarbage();

        while (true)
        {
            const std::optional<game_entity_id_type> primaryTargetId = monsterAggroComponent.GetPrimaryTarget();
            if (!primaryTargetId.has_value())
            {
                break;
            }

            if (!system.Get<SceneObjectSystem>().Contains(GamePlayer::TYPE, *primaryTargetId))
            {
                monsterAggroComponent.RemovePrimary();

                continue;
            }

            if (event.stateMachine.Transition(MonsterAIStateType::Combat))
            {
                co_return;
            }

            break;
        }

        const SceneObjectComponent& sceneObjectComponent = monster.GetSceneObjectComponent();
        if (sceneObjectComponent.IsMoving())
        {
            co_return;
        }

        const game_time_point_type now = GameTimeService::Now();
        if (now < _nextMoveTimePoint)
        {
            co_return;
        }

        const int32_t distance = std::uniform_int_distribution{ actionData.moveRangeMin, actionData.moveRangeMax }(randomEngine);
        const Eigen::Vector2f& position = sceneObjectComponent.GetPosition();

        Eigen::Vector2f destPos;

        if (PathFindingSystem * pathFindSystem = system.Find<PathFindingSystem>();
            !pathFindSystem || !pathFindSystem->GetRandPositionOnCircleOutLine(destPos, position, static_cast<float>(distance)))
        {
            const double angle = std::uniform_int_distribution{ 0, 360 }(randomEngine)*std::numbers::pi / 180.0;

            destPos = position;
            destPos.x() += static_cast<float>(std::cos(angle) * distance);
            destPos.y() += static_cast<float>(std::sin(angle) * distance);
        }

        if (const std::optional<GameMonsterSpawnerContext>& spawnContext = monster.GetSpawnerContext();
            spawnContext.has_value())
        {
            if (!spawnContext->spawnerArea.contains(destPos))
            {
                const Eigen::Vector2f direction = (spawnContext->spawnerCenter - position).normalized();

                destPos = position + direction * distance;
            }
        }

        const float speed = monster.GetMoveSpeed();

        system.Get<EntityMovementSystem>().MoveToPosition(monster, destPos, speed);

        int32_t moveDelay = std::uniform_int_distribution{ actionData.moveDelayMin, actionData.moveDelayMax }(randomEngine);
        moveDelay += static_cast<int32_t>(static_cast<float>(distance) / speed);

        _nextMoveTimePoint = now + std::chrono::milliseconds(moveDelay);
    }
}

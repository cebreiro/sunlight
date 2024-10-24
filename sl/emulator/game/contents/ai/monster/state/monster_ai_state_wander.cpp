#include "monster_ai_state_wander.h"

#include "sl/emulator/game/component/monster_aggro_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/ai/monster/monster_controller.h"
#include "sl/emulator/game/data/sox/monster_action.h"
#include "sl/emulator/game/data/sox/monster_base.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/entity_ai_control_system.h"
#include "sl/emulator/game/system/entity_movement_system.h"
#include "sl/emulator/game/system/entity_scan_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    MonsterAIStateWander::MonsterAIStateWander()
        : IState(MonsterAIStateType::Wander)
        , _mt(std::random_device{}())
    {
    }

    auto MonsterAIStateWander::OnEvent(const MonsterAIStateParam& event) -> Future<void>
    {
        EntityAIControlSystem& system = event.system;
        MonsterController& controller = event.controller;
        GameMonster& monster = event.monster;
        MonsterAggroComponent& monsterAggroComponent = monster.GetAggroComponent();

        const sox::MonsterAction& actionData = monster.GetData().GetAction();

        if (actionData.characteristic > 1 && actionData.scanTime > 0)
        {
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(GameTimeService::Now() - _lastScanTime);
            if (duration >= std::chrono::milliseconds(actionData.scanTime))
            {
                _playerScanResult.clear();
                co_await system.Get<EntityScanSystem>().ScanPlayerAsync(_playerScanResult, monster, static_cast<float>(actionData.sightRange));

                if (controller.ShouldStopCoroutine())
                {
                    co_return;
                }

                controller.ConfigureCoroutineExecutionContext();

                _lastScanTime = GameTimeService::Now();

                if (!_playerScanResult.empty())
                {
                    std::optional<game_entity_id_type> combatTargetId = std::nullopt;

                    while (!_playerScanResult.empty())
                    {
                        const auto iter = std::ranges::min_element(_playerScanResult,
                            [](const std::pair<game_entity_id_type, float>& lhs, const std::pair<game_entity_id_type, float>& rhs) -> bool
                            {
                                return lhs.first < rhs.first;
                            });
                        assert(iter != _playerScanResult.end());

                        if (const auto& entity = system.Get<SceneObjectSystem>().FindEntity(GamePlayer::TYPE, iter->first);
                            entity)
                        {
                            combatTargetId = entity->GetId();

                            break;
                        }

                        _playerScanResult.erase(iter);
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

        constexpr int32_t distance = 300;
        const double angle = std::uniform_int_distribution{ 0, 360 }(_mt) * std::numbers::pi / 180.0;

        Eigen::Vector2f destPos = sceneObjectComponent.GetPosition();
        destPos.x() += static_cast<float>(std::cos(angle) * distance);
        destPos.y() += static_cast<float>(std::sin(angle) * distance);

        const float speed = static_cast<float>(monster.GetData().GetBase().speed) / 100.f;

        system.Get<EntityMovementSystem>().MoveTo(monster, destPos, speed);
    }
}

#include "monster_ai_state_combat_flee.h"

#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/component/monster_aggro_component.h"
#include "sl/generator/game/contents/ai/monster/monster_controller.h"
#include "sl/generator/game/data/sox/monster_action.h"
#include "sl/generator/game/data/sox/monster_base.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    MonsterAIStateCombatFlee::MonsterAIStateCombatFlee()
        : IState(MonsterAIStateType::Combat)
    {
    }

    void MonsterAIStateCombatFlee::OnEnter()
    {
        _nextFleeTime = {};
    }

    auto MonsterAIStateCombatFlee::OnEvent(const MonsterAIStateParam& event) -> Future<void>
    {
        EntityAIControlSystem& system = event.system;
        MonsterController& controller = event.controller;
        MonsterAIStateMachine& stateMachine = event.stateMachine;
        GameMonster& monster = event.monster;

        const game_time_point_type now = GameTimeService::Now();
        if (now < _nextFleeTime)
        {
            co_return;
        }

        SceneObjectSystem& sceneObjectSystem = system.Get<SceneObjectSystem>();
        MonsterAggroComponent& aggroComponent = monster.GetAggroComponent();
        aggroComponent.RemoveGarbage();

        GameEntity* target = [&]() -> GameEntity*
            {
                while (!aggroComponent.Empty())
                {
                    const std::optional<game_entity_id_type> targetId = aggroComponent.GetPrimaryTarget();
                    if (!targetId.has_value())
                    {
                        return nullptr;
                    }

                    GameEntity* entity = sceneObjectSystem.FindEntity(*targetId);
                    if (!entity)
                    {
                        aggroComponent.RemovePrimary();
                    }

                    return entity;
                }

                return nullptr;
            }();

        if (!target)
        {
            stateMachine.Transition(MonsterAIStateType::Wander);

            co_return;
        }

        const Eigen::Vector2f& monsterPosition = monster.GetSceneObjectComponent().GetPosition();
        const Eigen::Vector2f& targetPosition = target->GetComponent<SceneObjectComponent>().GetPosition();
        const Eigen::Vector2f diff = (monsterPosition - targetPosition);

        if (diff.squaredNorm() >= 150.f * 150.f)
        {
            co_return;
        }

        const sox::MonsterAction& actionData = monster.GetData().GetAction();

        const int32_t moveDistance = controller.Rand(actionData.moveRangeMin, actionData.moveRangeMax);
        const int32_t moveDelay = controller.Rand(actionData.moveDelayMin, actionData.moveDelayMax);

        const float randAngle = static_cast<float>(controller.Rand(-60, 60)) * (static_cast<float>(std::numbers::pi) / 180.f);
        const float angle = std::atan2(diff.y(), diff.x()) + randAngle;

        const Eigen::Vector2f direction(std::cos(angle), std::sin(angle));
        const Eigen::Vector2f& moveVector = direction * static_cast<float>(moveDistance);

        const Eigen::Vector2f& destPosition = monsterPosition + moveVector;

        system.Get<EntityMovementSystem>().MoveToPosition(monster, destPosition, monster.GetMoveSpeed());

        _nextFleeTime = now + std::chrono::milliseconds(moveDelay);
    }
}

#include "monster_ai_state_combat.h"

#include "sl/generator/game/component/monster_aggro_component.h"
#include "sl/generator/game/component/monster_skill_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/ai/monster/monster_controller.h"
#include "sl/generator/game/data/sox/monster_action.h"
#include "sl/generator/game/data/sox/monster_base.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/entity_skill_effect_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    MonsterAIStateCombat::MonsterAIStateCombat()
        : IState(MonsterAIStateType::Combat)
    {
    }

    void MonsterAIStateCombat::OnEnter()
    {
        _targetId = std::nullopt;
        _attackIndex = std::nullopt;
        _nextChaseTimePoint = {};
    }

    auto MonsterAIStateCombat::OnEvent(const MonsterAIStateParam& event) -> Future<void>
    {
        EntityAIControlSystem& system = event.system;
        MonsterController& controller = event.controller;
        MonsterAIStateMachine& stateMachine = event.stateMachine;
        GameMonster& monster = event.monster;

        const sox::MonsterAction& actionData = monster.GetData().GetAction();
        SceneObjectSystem& sceneObjectSystem = system.Get<SceneObjectSystem>();

        GameEntity* target = nullptr;

        if (_targetId.has_value())
        {
            target = sceneObjectSystem.FindEntity(*_targetId);
        }

        if (!target)
        {
            target = GetPrimaryTarget(system, monster);

            if (target)
            {
                _targetId = target->GetId();
            }
        }

        const SceneObjectComponent& monsterSceneObjectComponent = monster.GetSceneObjectComponent();
        const Eigen::Vector2f& monsterPosition = monsterSceneObjectComponent.GetPosition();

        if (!target)
        {
            if (const float spawnPosDistanceSq = (monsterPosition - monster.GetSpawnPosition()).squaredNorm();
                spawnPosDistanceSq > static_cast<float>(actionData.domain * actionData.domain))
            {
                stateMachine.Transition(MonsterAIStateType::Return);
            }
            else
            {
                stateMachine.Transition(MonsterAIStateType::Wander);
            }

            co_return;
        }

        if (!actionData.canGoOut)
        {
            if (const float spawnPosDistanceSq = (monsterPosition - monster.GetSpawnPosition()).squaredNorm();
                spawnPosDistanceSq > static_cast<float>(actionData.domain * actionData.domain))
            {
                stateMachine.Transition(MonsterAIStateType::Return);

                co_return;
            }
        }

        const SceneObjectComponent& targetSceneObjectComponent = target->GetComponent<SceneObjectComponent>();

        if (const float targetPosDistanceSq = (monsterPosition - targetSceneObjectComponent.GetPosition()).squaredNorm();
            targetPosDistanceSq > static_cast<float>(actionData.chaseRange * actionData.chaseRange))
        {
            stateMachine.Transition(MonsterAIStateType::Return);

            co_return;
        }

        if (!_attackIndex.has_value())
        {
            _attackIndex = [&]() -> int32_t
                {
                    const MonsterAttackData& attackData = monster.GetData().GetAttack();

                    if (attackData.skills.empty())
                    {
                        return 0;
                    }

                    const MonsterSkillComponent& skillComponent = monster.GetSkillComponent();

                    boost::container::static_vector<std::pair<int32_t, int32_t>, 3> candidates;
                    int32_t totalWeight = 0;

                    for (int32_t i = 0; i < 3; ++i)
                    {
                        if (i >= std::ssize(attackData.skills))
                        {
                            break;
                        }

                        const MonsterAttackData::Skill& skillData = attackData.skills[i];
                        
                        if (skillComponent.CanUseSkill(skillData.id))
                        {
                            totalWeight += skillData.percent;

                            candidates.emplace_back(i + 1, skillData.percent);
                        }
                    }

                    if (candidates.empty())
                    {
                        return 0;
                    }

                    if (controller.Rand(0, 100) <= attackData.percent)
                    {
                        return 0;
                    }

                    const int32_t candidateCount = static_cast<int32_t>(std::ssize(candidates));

                    if (candidateCount == 1)
                    {
                        return candidates[0].first;
                    }

                    const int32_t value = controller.Rand(1, totalWeight);

                    for (const auto& [id, weight] : candidates)
                    {
                        if (value <= weight)
                        {
                            return id;
                        }
                    }

                    return 0;
                }();
        }

        const int32_t range = [&]() -> int32_t
            {
                assert(_attackIndex.has_value());

                const MonsterAttackData& attackData = monster.GetData().GetAttack();

                if (*_attackIndex == 0)
                {
                    return attackData.range;
                }

                assert(std::ssize(attackData.skills) > (*_attackIndex) - 1);

                return attackData.skills[*_attackIndex - 1].range;
            }();

        const Eigen::Vector2f& targetPosition = targetSceneObjectComponent.GetPosition();

        const int32_t distance = static_cast<int32_t>((monsterPosition - targetPosition).norm())
            - targetSceneObjectComponent.GetBodySize() - monsterSceneObjectComponent.GetBodySize();

        if (distance <= range)
        {
            const MonsterAttackData& attackData = monster.GetData().GetAttack();

            const int32_t attackIndex = *std::exchange(_attackIndex, std::nullopt);
            if (attackIndex == 0)
            {
                system.Get<EntitySkillEffectSystem>().ProcessMonsterNormalAttack(monster, *target);

                co_await Delay(std::chrono::milliseconds(std::max(attackData.attackEndFrame + 500, attackData.attackDelay)));
            }
            else
            {
                const int32_t index = attackIndex - 1;
                assert(index < std::ssize(attackData.skills));

                const MonsterAttackData::Skill& skillData = attackData.skills[index];

                monster.GetSkillComponent().SetCoolTime(skillData.id, GameTimeService::Now() + std::chrono::milliseconds(skillData.coolTime));

                system.Get<EntitySkillEffectSystem>().ProcessMonsterSkill(monster, *target, attackData.skills[index], attackIndex);

                co_await Delay(std::chrono::milliseconds(std::max(skillData.skillEndFrame + 500, skillData.skillDelay)));
            }

            co_return;
        }

        const game_time_point_type now = GameTimeService::Now();
        if (now < _nextChaseTimePoint)
        {
            co_return;
        }

        system.Get<EntityMovementSystem>().MoveToTarget(monster, *target, monster.GetChaseSpeed());

        _nextChaseTimePoint = now + std::chrono::milliseconds(actionData.moveDelayChase);

        co_return;
    }

    auto MonsterAIStateCombat::GetPrimaryTarget(EntityAIControlSystem& system, GameMonster& monster) -> GameEntity*
    {
        SceneObjectSystem& sceneObjectSystem = system.Get<SceneObjectSystem>();
        MonsterAggroComponent& monsterAggroComponent = monster.GetAggroComponent();

        while (!monsterAggroComponent.Empty())
        {
            const std::optional<game_entity_id_type> targetId = monsterAggroComponent.GetPrimaryTarget();
            if (!targetId.has_value())
            {
                return nullptr;
            }

            GameEntity* entity = sceneObjectSystem.FindEntity(*targetId);
            if (!entity)
            {
                monsterAggroComponent.RemovePrimary();

                continue;
            }

            return entity;
        }

        return nullptr;
    }
}

#include "monster_skill_effect_system.h"

#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/sector/game_spatial_sector.h"
#include "sl/emulator/game/contents/state/game_entity_state.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/system/entity_damage_system.h"
#include "sl/emulator/game/system/entity_movement_system.h"
#include "sl/emulator/game/system/entity_scan_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/zone_timer_service.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/emulator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    MonsterSkillEffectSystem::MonsterSkillEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
    {
    }

    void MonsterSkillEffectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<EntityScanSystem>());
        Add(stage.Get<EntityMovementSystem>());
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityDamageSystem>());
    }

    bool MonsterSkillEffectSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto MonsterSkillEffectSystem::GetName() const -> std::string_view
    {
        return "monster_skill_effect_system";
    }

    auto MonsterSkillEffectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<MonsterSkillEffectSystem>();
    }

    void MonsterSkillEffectSystem::ProcessNormalAttack(GameMonster& monster, GameEntity& target, int32_t attackIndex)
    {
        const MonsterData& data = monster.GetData();
        const MonsterAttackData& attackData = data.GetAttack();

        std::vector<game_entity_id_type> targets;

        if (!_attackTargetRecycleBuffer.empty())
        {
            targets = std::move(_attackTargetRecycleBuffer.back());
            targets.clear();

            _attackTargetRecycleBuffer.pop_back();
        }

        const Eigen::Vector2f& targetPosition = target.GetComponent<SceneObjectComponent>().GetPosition();

        if (attackData.range > 0)
        {
            const float range = static_cast<float>(attackData.range);

            Get<EntityScanSystem>().ScanMonsterAttackTarget(targets, targetPosition, range);
        }
        else
        {
            targets.emplace_back(target.GetId());
        }

        SceneObjectComponent& sceneObjectComponent = monster.GetSceneObjectComponent();
        sceneObjectComponent.SetMoving(false);
        sceneObjectComponent.SetDestPosition(sceneObjectComponent.GetPosition());
        sceneObjectComponent.SetYaw(CalculateYaw(sceneObjectComponent.GetPosition(), targetPosition));

        EntityStateComponent& stateComponent = monster.GetStateComponent();
        stateComponent.SetState(GameEntityState{
                .type = GameEntityStateType::NormalAttack,
                .skillId = attackIndex,
            });

        Get<EntityViewRangeSystem>().VisitPlayer(monster, [&monster](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(monster));
                player.Defer(SceneObjectPacketCreator::CreateState(monster));
                player.FlushDeferred();
            });

        if (targets.empty())
        {
            _attackTargetRecycleBuffer.emplace_back(std::move(targets));

            assert(false);

            return;
        }

        _serviceLocator.Get<ZoneTimerService>().AddTimer(std::chrono::milliseconds(attackData.attackBeatFrame),
            [this, mobId = monster.GetId(), targets = std::move(targets)]() mutable
            {
                SceneObjectSystem& sceneObjectSystem = Get<SceneObjectSystem>();

                GameEntity* entity = sceneObjectSystem.FindEntity(GameMonster::TYPE, mobId);
                if (!entity || entity->GetId().GetRecycleSequence() != mobId.GetRecycleSequence())
                {
                    return;
                }

                EntityDamageSystem& entityDamageSystem = Get<EntityDamageSystem>();
                GameMonster& monster = *entity->Cast<GameMonster>();

                for (game_entity_id_type targetId : targets)
                {
                    GameEntity* target = sceneObjectSystem.FindEntity(targetId);
                    if (!target || target->GetId().GetRecycleSequence() != targetId.GetRecycleSequence())
                    {
                        continue;
                    }

                    entityDamageSystem.ProcessMonsterNormalAttack(monster, *target);
                }

                _attackTargetRecycleBuffer.emplace_back(std::move(targets));
            });
    }

    auto MonsterSkillEffectSystem::CalculateYaw(const Eigen::Vector2f& monsterPosition, const Eigen::Vector2f& mainTargetPosition) -> float
    {
        const Eigen::Vector2f vector = mainTargetPosition - monsterPosition;

        return static_cast<float>(std::atan2(vector.y(), vector.x()) * (180.0 / std::numbers::pi));
    }
}

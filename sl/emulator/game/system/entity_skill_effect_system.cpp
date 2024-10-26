#include "entity_skill_effect_system.h"

#include "sl/data/abf/ability_value.h"
#include "sl/emulator/game/component/entity_passive_effect_component.h"
#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/monster_stat_component.h"
#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/damage/damage_result.h"
#include "sl/emulator/game/contents/passive/passive.h"
#include "sl/emulator/game/contents/passive/effect/passive_effect_factory.h"
#include "sl/emulator/game/contents/passive/effect/passive_effect_interface.h"
#include "sl/emulator/game/contents/passive/effect/impl/passive_effect_stat.h"
#include "sl/emulator/game/contents/skill/skill_target_selector.h"
#include "sl/emulator/game/contents/stat/player_stat_type.h"
#include "sl/emulator/game/contents/state/game_entity_state.h"
#include "sl/emulator/game/data/sox/item_weapon.h"
#include "sl/emulator/game/data/sox/motion_data.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/message/creator/status_message_creator.h"
#include "sl/emulator/game/system/entity_damage_system.h"
#include "sl/emulator/game/system/entity_movement_system.h"
#include "sl/emulator/game/system/entity_scan_system.h"
#include "sl/emulator/game/system/entity_status_effect_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/zone_timer_service.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data.h"
#include "sl/emulator/service/gamedata/monster/monster_attack_data.h"
#include "sl/emulator/service/gamedata/monster/monster_data.h"
#include "sl/emulator/service/gamedata/motion/player_motion_data_provider.h"
#include "sl/emulator/service/gamedata/skill/player_skill_data.h"
#include "sl/emulator/service/gamedata/skill/skill_data_provider.h"

namespace sunlight
{
    EntitySkillEffectSystem::EntitySkillEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
        , _skillTargetSelector(std::make_unique<SkillTargetSelector>(*this))
    {
    }

    EntitySkillEffectSystem::~EntitySkillEffectSystem()
    {
    }

    void EntitySkillEffectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerIndexSystem>());
        Add(stage.Get<EntityStatusEffectSystem>());
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<EntityDamageSystem>());
        Add(stage.Get<EntityScanSystem>());
        Add(stage.Get<EntityMovementSystem>());
    }

    bool EntitySkillEffectSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto EntitySkillEffectSystem::GetName() const -> std::string_view
    {
        return "player_skill_effect_system";
    }

    auto EntitySkillEffectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntitySkillEffectSystem>();
    }

    void EntitySkillEffectSystem::OnStageEnter(GamePlayer& player, StageEnterType type)
    {
        if (type != StageEnterType::Login)
        {
            return;
        }

        for (PlayerSkill& skill : player.GetSkillComponent().GetSkills())
        {
            OnSkillAdd(player, skill);
        }

        Get<PlayerStatSystem>().UpdateRegenStat(player);
    }

    void EntitySkillEffectSystem::OnSkillAdd(GamePlayer& player, PlayerSkill& skill)
    {
        const PlayerSkillData& skillData = skill.GetData();

        if (!skillData.passive)
        {
            return;
        }
            
        auto passive = std::make_unique<Passive>(skill.GetId(), skill.GetLevel());

        if (skillData.effectWeaponClassCondition)
        {
            passive->SetWeaponClassCondition(skillData.effectWeaponClassCondition->weaponClass);
        }

        if (skillData.effectRangeWeaponCondition)
        {
            passive->SetRangeWeaponCondition(true);
        }

        if (skillData.effectAttackProbabilityCondition)
        {
            const int32_t baseValue = skillData.effectAttackProbabilityCondition->baseProbability;
            const int32_t valuePerSkillLevel = skillData.effectAttackProbabilityCondition->probabilityPerLevel;

            passive->SetAttackEffectProbability(baseValue, valuePerSkillLevel);
        }

        for (const SkillEffectData& skillEffectData : skillData.effects)
        {
            if (std::unique_ptr<IPassiveEffect> effect = PassiveEffectFactory::CreateEffect(skillEffectData);
                effect)
            {
                passive->AddEffect(std::move(effect));
            }
        }

        const GameItem* weaponItem = player.GetItemComponent().GetEquipmentItem(EquipmentPosition::Weapon1);

        const WeaponClassType weaponClass = weaponItem
            ? static_cast<WeaponClassType>(weaponItem->GetData().GetWeaponData()->weaponClass)
            : WeaponClassType::None;

        if (passive->CanBeActivatedBy(weaponClass))
        {
            passive->SetActive(true);

            EntityPassiveEffectComponent& passiveEffectComponent = player.GetPassiveEffectComponent();

            for (IPassiveEffect& effect : passive->GetEffectRange())
            {
                Apply(player, effect, skill.GetLevel());

                passiveEffectComponent.AddEffect(skill.GetId(), &effect);
            }
        }

        skill.SetPassive(std::move(passive));
        Get<PlayerStatSystem>().UpdateRegenStat(player);
    }

    void EntitySkillEffectSystem::OnSkillLevelChange(GamePlayer& player, const PlayerSkill& skill, int32_t oldLevel, int32_t newLevel)
    {
        if (!skill.HasPassive())
        {
            return;
        }

        const Passive& passive = skill.GetPassive();
        if (!passive.IsActive())
        {
            return;
        }

        for (IPassiveEffect& effect : passive.GetEffectRange())
        {
            Revert(player, effect, oldLevel);
            Apply(player, effect, newLevel);
        }

        Get<PlayerStatSystem>().UpdateRegenStat(player);
    }

    void EntitySkillEffectSystem::OnMainWeaponChange(GamePlayer& player)
    {
        const GameItem* weaponItem = player.GetItemComponent().GetEquipmentItem(EquipmentPosition::Weapon1);

        const WeaponClassType weaponClass = weaponItem
            ? static_cast<WeaponClassType>(weaponItem->GetData().GetWeaponData()->weaponClass)
            : WeaponClassType::None;


        for (PlayerSkill& skill : player.GetSkillComponent().GetSkills())
        {
            if (!skill.HasPassive())
            {
                continue;
            }

            Passive& passive = skill.GetPassive();

            if (const bool change = passive.IsActive() != passive.CanBeActivatedBy(weaponClass);
                !change)
            {
                continue;
            }

            EntityPassiveEffectComponent& passiveEffectComponent = player.GetPassiveEffectComponent();

            if (passive.IsActive())
            {
                for (IPassiveEffect& effect : passive.GetEffectRange())
                {
                    Revert(player, effect, skill.GetLevel());
                }

                passiveEffectComponent.RemoveEffects(skill.GetId());
            }
            else
            {
                for (IPassiveEffect& effect : passive.GetEffectRange())
                {
                    Apply(player, effect, skill.GetLevel());

                    passiveEffectComponent.AddEffect(skill.GetId(), &effect);
                }
            }

            passive.SetActive(!passive.IsActive());
        }
    }

    void EntitySkillEffectSystem::OnSkillUse(GamePlayer& player, const GameEntityState& state)
    {
        const int32_t skillId = state.skillId;
        const GameEntityType targetType = state.targetType;
        const int32_t chargeTime = state.param1;
        const int32_t chargeCount = state.param2;
        const WeaponClassType weaponClass = static_cast<WeaponClassType>(state.motionId);

        (void)chargeTime;

        PlayerSkillComponent& skillComponent = player.GetSkillComponent();

        PlayerSkill* skill = skillComponent.FindSkill(skillId);
        if (!skill)
        {
            return;
        }

        const PlayerSkillData& skillData = skill->GetData();

        if (skillData.applyCasting)
        {
            if (state.param1 <= 0)
            {
                // state.param1 < 0 -> cancel
                // state.param1 = 0 -> start

                return;
            }
        }

        GameEntity* mainTarget = targetType != GameEntityType::None ? Get<SceneObjectSystem>().FindEntity(targetType, state.targetId) : nullptr;

        SkillTargetSelector::result_type skillTargets;
        if (!_skillTargetSelector->SelectTarget(skillTargets, player, skillData, mainTarget))
        {
            player.Notice(fmt::format("fail to select skill target. skill: {}", skillId));

            return;
        }

        auto applySkillEffect = [this, attackId = state.attackId, skillId, weaponClass, chargeCount, skillTargets](GamePlayer& player, const AbilityValue* abilityValue) mutable
            {
                PlayerSkillComponent& skillComponent = player.GetSkillComponent();
                PlayerSkill* skill = skillComponent.FindSkill(skillId);

                if (!skill)
                {
                    return;
                }

                for (const game_entity_id_type targetId : skillTargets)
                {
                    player.Notice(fmt::format("skill: {}, target: {}", skillId, targetId));
                }

                SceneObjectSystem& sceneObjectSystem = Get<SceneObjectSystem>();

                for (const SkillEffectData& skillEffect : skill->GetData().effects)
                {
                    switch (skillEffect.category)
                    {
                    case SkillEffectCategory::Damage:
                    {
                        for (const game_entity_id_type targetId : skillTargets)
                        {
                            GameEntity* target = sceneObjectSystem.FindEntity(GameMonster::TYPE, targetId);
                            if (!target || target->GetId().GetRecycleSequence() != targetId.GetRecycleSequence())
                            {
                                continue;
                            }

                            Get<EntityDamageSystem>().ProcessPlayerSkillEffect(player, *target->Cast<GameMonster>(), *skill, skillEffect,
                                attackId, chargeCount, weaponClass, abilityValue);
                        }
                    }
                    break;
                    case SkillEffectCategory::StatusEffect:
                    {
                        Get<EntityStatusEffectSystem>().AddStatusEffectBySkill(skillId, skill->GetLevel(), skillTargets, skillEffect);
                    }
                    break;
                    case SkillEffectCategory::Summon:
                    case SkillEffectCategory::JobEffect:
                    case SkillEffectCategory::Stat:
                    default:;
                    }
                }
            };

        if (const auto iter = skillData.effectAttackValues.find(weaponClass);
            iter != skillData.effectAttackValues.end())
        {
            for (const AbilityValue& abilityValue : iter->second | notnull::reference)
            {
                if (abilityValue.begin == 0)
                {
                    applySkillEffect(player, &abilityValue);
                }
                else
                {
                    _serviceLocator.Get<ZoneTimerService>().AddTimer(std::chrono::milliseconds(abilityValue.begin),
                        player.GetCId(), _stageId, [applySkillEffect, &abilityValue](GamePlayer& player) mutable
                        {
                            applySkillEffect(player, &abilityValue);
                        });
                }
            }
        }
        else
        {
            applySkillEffect(player, nullptr);
        }
    }

    void EntitySkillEffectSystem::OnNormalAttackUse(GamePlayer& player, const GameEntityState& state)
    {
        const int32_t weaponMotion = player.GetAppearanceComponent().GetWeaponMotionCategory();

        const sox::MotionData* motionData = _serviceLocator.Get<GameDataProvideService>().GetPlayerMotionDataProvider().FindNormalAttackMotion(
            weaponMotion == 0 ? 1 : weaponMotion, state.motionId);
        if (!motionData)
        {
            return;
        }

        GameEntity* target = Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, state.targetId);
        if (!target)
        {
            return;
        }

        if (const auto* statComponent = target->FindComponent<MonsterStatComponent>();
            !statComponent || statComponent->IsDead())
        {
            return;
        }

        const auto processNormalAttack = [this, targetId = target->GetId(), attackId = state.attackId, motionData](GamePlayer& player)
            {
                GameEntity* target = Get<SceneObjectSystem>().FindEntity(GameEntityType::Enemy, targetId);
                if (!target)
                {
                    return;
                }

                Get<EntityDamageSystem>().ProcessPlayerNormalAttack(player, *target->Cast<GameMonster>(), attackId, player.GetItemComponent().GetWeaponClass(), *motionData);
            };

        if (motionData->hitTime == 0)
        {
            processNormalAttack(player);
        }
        else
        {
            _serviceLocator.Get<ZoneTimerService>().AddTimer(
                std::chrono::milliseconds(motionData->hitTime), player.GetCId(), _stageId, processNormalAttack);
        }
    }

    void EntitySkillEffectSystem::ProcessMonsterNormalAttack(GameMonster& monster, GameEntity& target, int32_t attackIndex)
    {
        if (monster.GetStatComponent().IsDead())
        {
            return;
        }

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

        Get<EntityMovementSystem>().Remove(monster.GetId());

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

                if (monster.GetStatComponent().IsDead())
                {
                    return;
                }

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

    void EntitySkillEffectSystem::ProcessMonsterSkill(GameMonster& monster, GameEntity& target, const MonsterAttackData::Skill& attackData, int32_t attackIndex)
    {
        if (monster.GetStatComponent().IsDead())
        {
            return;
        }

        const SkillDataProvider& skillDataProvider = _serviceLocator.Get<GameDataProvideService>().GetSkillDataProvider();
        const MonsterSkillData* skillData = skillDataProvider.FindMonsterSkill(attackData.id);

        if (!skillData)
        {
            ProcessMonsterNormalAttack(monster, target, attackIndex);

            return;
        }

        Get<EntityMovementSystem>().Remove(monster.GetId());

        const Eigen::Vector2f& targetPosition = target.GetComponent<SceneObjectComponent>().GetPosition();

        SceneObjectComponent& sceneObjectComponent = monster.GetSceneObjectComponent();
        sceneObjectComponent.SetMoving(false);
        sceneObjectComponent.SetDestPosition(sceneObjectComponent.GetPosition());
        sceneObjectComponent.SetYaw(CalculateYaw(sceneObjectComponent.GetPosition(), targetPosition));

        EntityStateComponent& stateComponent = monster.GetStateComponent();
        stateComponent.SetState(GameEntityState{
                .type = GameEntityStateType::PlaySkill,
                .param2 = attackIndex,
                .skillId = attackData.id,
            });

        Get<EntityViewRangeSystem>().VisitPlayer(monster, [&monster](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(monster));
                player.Defer(SceneObjectPacketCreator::CreateState(monster));
                player.FlushDeferred();
            });

        SkillTargetSelector::result_type skillTargets;
        if (!_skillTargetSelector->SelectTarget(skillTargets, monster, *skillData, target))
        {
            return;
        }

        auto applySkillEffect = [this, skillData, skillLevel = attackData.level, skillTargets](GameMonster& monster, const AbilityValue* abilityValue) mutable
            {
                if (monster.GetStatComponent().IsDead())
                {
                    return;
                }

                SceneObjectSystem& sceneObjectSystem = Get<SceneObjectSystem>();

                for (const SkillEffectData& skillEffect : skillData->effects)
                {
                    switch (skillEffect.category)
                    {
                    case SkillEffectCategory::Damage:
                    {
                        for (const game_entity_id_type targetId : skillTargets)
                        {
                            GameEntity* target = sceneObjectSystem.FindEntity(targetId);
                            if (!target || target->GetId().GetRecycleSequence() != targetId.GetRecycleSequence())
                            {
                                continue;
                            }

                            Get<EntityDamageSystem>().ProcessMonsterSkillEffect(monster, *target, *skillData, skillEffect, abilityValue);
                        }
                    }
                    break;
                    case SkillEffectCategory::StatusEffect:
                    {
                        Get<EntityStatusEffectSystem>().AddStatusEffectBySkill(skillData->index, skillLevel, skillTargets, skillEffect);
                    }
                    break;
                    default:;
                    }
                }
            };

        if (skillData->effectAttackValues.empty())
        {
            applySkillEffect(monster, nullptr);
        }
        else
        {
            for (const AbilityValue& abilityValue : skillData->effectAttackValues | notnull::reference)
            {
                const int32_t beatFrame = std::max(abilityValue.begin, abilityValue.begin);
                if (beatFrame == 0)
                {
                    applySkillEffect(monster, &abilityValue);
                }
                else
                {
                    _serviceLocator.Get<ZoneTimerService>().AddTimer(std::chrono::milliseconds(beatFrame),
                        [this, applySkillEffect, &abilityValue, mobId = monster.GetId()]() mutable
                        {
                            GameEntity* entity = Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, mobId);
                            if (!entity || entity->GetId().GetRecycleSequence() != mobId.GetRecycleSequence())
                            {
                                return;
                            }

                            applySkillEffect(*entity->Cast<GameMonster>(), &abilityValue);
                        });
                }
            }
        }
    }

    void EntitySkillEffectSystem::Apply(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const
    {
        const PassiveEffectType type = passiveEffect.GetType();
        if (type == PassiveEffectType::Stat)
        {
            PassiveEffectStat* statEffect = Cast<PassiveEffectStat>(passiveEffect);
            assert(statEffect);

            if (const auto statType = static_cast<PlayerStatType>(statEffect->GetStatType());
                IsValid(statType))
            {
                const SkillEffectData& data = statEffect->GetData();

                // client 0x4B02BB
                // v5[319] -> value1
                // v5[320] -> value2
                // ...

                const int32_t fixedValue = data.value2 + skillLevel * data.value1;
                double percentageValue = (data.value4 + skillLevel * data.value3) / 100.0;

                PlayerStatComponent& statComponent = player.GetStatComponent();

                statComponent.AddStat(statType, StatOriginType::SkillPassive, fixedValue);
                statComponent.AddStat(statType, StatOriginType::SkillPassivePercentage, percentageValue);

                statEffect->SetStatValue(fixedValue);
                statEffect->SetStatPercentageValue(percentageValue);
            }
        }
    }

    void EntitySkillEffectSystem::Revert(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const
    {
        (void)skillLevel;

        const PassiveEffectType type = passiveEffect.GetType();
        if (type == PassiveEffectType::Stat)
        {
            PassiveEffectStat* statEffect = Cast<PassiveEffectStat>(passiveEffect);
            assert(statEffect);

            if (const auto statType = static_cast<PlayerStatType>(statEffect->GetStatType());
                IsValid(statType))
            {
                PlayerStatComponent& statComponent = player.GetStatComponent();

                statComponent.AddStat(statType, StatOriginType::SkillPassive, -statEffect->GetStatValue());
                statComponent.AddStat(statType, StatOriginType::SkillPassivePercentage, -statEffect->GetStatPercentageValue());

                statEffect->SetStatValue(0);
                statEffect->SetStatPercentageValue(0.0);
            }
        }
    }

    auto EntitySkillEffectSystem::CalculateYaw(const Eigen::Vector2f& src, const Eigen::Vector2f& dest) -> float
    {
        const Eigen::Vector2f vector = dest - src;

        return static_cast<float>(std::atan2(vector.y(), vector.x()) * (180.0 / std::numbers::pi));
    }
}

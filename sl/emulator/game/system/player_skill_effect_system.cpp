#include "player_skill_effect_system.h"

#include "sl/emulator/game/component/entity_passive_effect_component.h"
#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/contents/attack/attack_result.h"
#include "sl/emulator/game/contents/passive/passive.h"
#include "sl/emulator/game/contents/passive/effect/passive_effect_factory.h"
#include "sl/emulator/game/contents/passive/effect/passive_effect_interface.h"
#include "sl/emulator/game/contents/passive/effect/impl/passive_effect_stat.h"
#include "sl/emulator/game/contents/skill/player_skill_target_selector.h"
#include "sl/emulator/game/contents/stat/player_stat_type.h"
#include "sl/emulator/game/contents/state/game_entity_state.h"
#include "sl/emulator/game/data/sox/item_weapon.h"
#include "sl/emulator/game/data/sox/motion_data.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/status_message_creator.h"
#include "sl/emulator/game/system/entity_status_effect_system.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/zone_timer_service.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data.h"
#include "sl/emulator/service/gamedata/motion/player_motion_data_provider.h"
#include "sl/emulator/service/gamedata/skill/player_skill_data.h"

namespace sunlight
{
    PlayerSkillEffectSystem::PlayerSkillEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
        , _skillTargetSelector(std::make_unique<PlayerSkillTargetSelector>(*this))
    {
    }

    PlayerSkillEffectSystem::~PlayerSkillEffectSystem()
    {
    }

    void PlayerSkillEffectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerIndexSystem>());
        Add(stage.Get<EntityStatusEffectSystem>());
        Add(stage.Get<PlayerStatSystem>());
    }

    bool PlayerSkillEffectSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto PlayerSkillEffectSystem::GetName() const -> std::string_view
    {
        return "player_skill_effect_system";
    }

    auto PlayerSkillEffectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerSkillEffectSystem>();
    }

    void PlayerSkillEffectSystem::OnStageEnter(GamePlayer& player, StageEnterType type)
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

    void PlayerSkillEffectSystem::OnSkillAdd(GamePlayer& player, PlayerSkill& skill)
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

    void PlayerSkillEffectSystem::OnSkillLevelChange(GamePlayer& player, const PlayerSkill& skill, int32_t oldLevel, int32_t newLevel)
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

    void PlayerSkillEffectSystem::OnMainWeaponChange(GamePlayer& player)
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

    void PlayerSkillEffectSystem::OnSkillUse(GamePlayer& player, const GameEntityState& state)
    {
        const int32_t skillId = state.skillId;
        const game_entity_id_type targetId = state.targetId;
        const GameEntityType targetType = state.targetType;
        const int32_t chargeTime = state.param1;

        (void)chargeTime;

        PlayerSkillComponent& skillComponent = player.GetSkillComponent();

        PlayerSkill* skill = skillComponent.FindSkill(skillId);
        if (!skill)
        {
            return;
        }

        if (skill->GetData().applyCasting)
        {
            if (state.param1 <= 0)
            {
                // state.param1 < 0 -> cancel
                // state.param1 = 0 -> start

                return;
            }
        }

        GameEntity* mainTarget = targetType != GameEntityType::None ? Get<SceneObjectSystem>().FindEntity(targetType, targetId).get() : nullptr;

        PlayerSkillTargetSelector::result_type skillTargets;
        if (!_skillTargetSelector->SelectTarget(skillTargets, player, skill->GetData(), mainTarget))
        {
            player.Notice(fmt::format("fail to select skill target. skill: {}", skillId));

            return;
        }

        for (GameEntity* target : skillTargets)
        {
            player.Notice(fmt::format("skill: {}, target: [{}, {}]", skillId, target->GetId(), ToString(target->GetType())));
        }

        const PlayerSkillData& data = skill->GetData();

        for (const SkillEffectData& skillEffect : data.effects)
        {
            switch (skillEffect.category)
            {
            case SkillEffectCategory::Damage:
            {
                const auto weaponClass = player.GetItemComponent().GetWeaponClass();

                const AttackResult result{
                    .attackerId = player.GetId(),
                    .attackerType = player.GetType(),
                    .damageType = AttackDamageType::DamageMonster,
                    .id = state.attackId,
                    .motionId = 3,
                    .skillId = skillId,
                    .weaponClass = weaponClass,
                    .damage = 1234,
                    .damageCount = 1,
                    .damageInterval = 0,
                    .attackBlowGroup = 0,
                    .attackTargetBlowType = AttackTargetBlowType::BlowSmall,
                    .attackedResultType = AttackedResultType::Damage_A,
                };

                for (GameEntity* target : skillTargets)
                {
                    Get<EntityViewRangeSystem>().VisitPlayer(*target, [target , &result](GamePlayer& player)
                        {
                            player.Send(StatusMessageCreator::CreateAttackResult(*target, result));
                        });
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
    }

    void PlayerSkillEffectSystem::OnNormalAttackUse(GamePlayer& player, const GameEntityState& state)
    {
        const int32_t weaponMotion = player.GetAppearanceComponent().GetWeaponMotionCategory();

        const sox::MotionData* motionData = _serviceLocator.Get<GameDataProvideService>().GetPlayerMotionDataProvider().FindNormalAttackMotion(
            weaponMotion == 0 ? 1 : weaponMotion, state.motionId);
        if (!motionData)
        {
            return;
        }

        const auto findTargetAndProcess = [this, targetId = state.targetId, attackId = state.attackId, motionData](GamePlayer& player)
            {
                const auto& target = Get<SceneObjectSystem>().FindEntity(GameEntityType::Enemy, targetId);
                if (!target)
                {
                    return;
                }

                ProcessNormalAttack(player, *target->Cast<GameMonster>(), attackId , *motionData);
            };

        if (motionData->hitTime == 0)
        {
            findTargetAndProcess(player);
        }
        else
        {
            _serviceLocator.Get<ZoneTimerService>().AddTimer(
                std::chrono::milliseconds(motionData->hitTime), player.GetCId(), _stageId, findTargetAndProcess);
        }
    }

    void PlayerSkillEffectSystem::ProcessNormalAttack(GamePlayer& player, GameMonster& monster, int32_t attackId, const sox::MotionData& motionData)
    {
        (void)motionData;

        const AttackResult result{
            .attackerId = player.GetId(),
            .attackerType = player.GetType(),
            .damageType = AttackDamageType::DamageMonster,
            .id = attackId,
            .motionId = 3,
            .skillId = 0,
            .weaponClass = player.GetItemComponent().GetWeaponClass(),
            .damage = 1234,
            .damageCount = 1,
            .damageInterval = 0,
            .attackBlowGroup = 0,
            .attackTargetBlowType = AttackTargetBlowType::BlowSmall,
            .attackedResultType = AttackedResultType::Damage_A,
        };

        Get<EntityViewRangeSystem>().VisitPlayer(monster, [&monster, &result](GamePlayer& player)
            {
                player.Send(StatusMessageCreator::CreateAttackResult(monster, result));
            });
    }

    void PlayerSkillEffectSystem::Apply(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const
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

    void PlayerSkillEffectSystem::Revert(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const
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
}

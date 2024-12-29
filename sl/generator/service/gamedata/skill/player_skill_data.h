#pragma once
#include <boost/container/static_vector.hpp>
#include "sl/generator/service/gamedata/skill/skill_effect_data.h"
#include "sl/generator/service/gamedata/skill/skill_target_area_type.h"
#include "sl/generator/service/gamedata/skill/skill_target_select_type.h"

namespace sunlight::sox
{
    struct SkillBasic;
}

namespace sunlight
{
    struct AbilityValue;
}

namespace sunlight
{
    struct PlayerSkillData
    {
        struct PrevSkill
        {
            int32_t id = 0;
            int32_t level = 0;
        };

        PlayerSkillData(const PlayerSkillData& other) = delete;
        PlayerSkillData& operator=(const PlayerSkillData& other) = delete;

        PlayerSkillData() = default;
        PlayerSkillData(PlayerSkillData&& other) noexcept = default;
        PlayerSkillData& operator=(PlayerSkillData&& other) noexcept = default;

        int32_t index = 0;
        int32_t routineId = 0;
        int32_t abilityType = 0;
        boost::container::static_vector<int32_t, 3> jobs;
        int32_t degree = 0;
        int32_t maxLevel = 0;
        int32_t delayId = 0;
        int32_t canSubJob = 0;
        int32_t levelQualification = 0;
        boost::container::static_vector<PrevSkill, 2> prevSkills;
        bool passive = false;
        int32_t spConsumption = 0;
        int32_t spConsumptionDelay = 0;
        int32_t furyOnly = 0;
        int32_t requireItem = 0;
        int32_t requireItemCount = 0;
        float motionSpeed = 0;
        int32_t needWeapon = 0;
        int32_t delayType = 0;
        int32_t delayTime = 0;
        int32_t useType = 0;
        SkillTargetSelectType applyTargetType = {};
        SkillTargetingAreaType applyDamageType = {};
        int32_t damageLength = 0;
        int32_t damageLength2 = 0;
        int32_t damageMaxCount = 0;
        int32_t cancelable = 0;
        int32_t useRange = 0;
        int32_t addWeaponRange = 0;
        int32_t applyCasting = 0;
        int32_t castingDelay = 0;
        int32_t applyCharging = 0;
        int32_t chargingDelay = 0;
        int32_t damageMotionType = 0;
        boost::container::static_vector<SkillEffectData, 4> effects;
        std::unordered_map<WeaponClassType, boost::container::static_vector<PtrNotNull<const AbilityValue>, 4>> effectAttackValues;

        std::optional<SkillEffectWeaponCondition> effectWeaponClassCondition = std::nullopt;
        std::optional<SkillEffectRangeWeaponCondition> effectRangeWeaponCondition = std::nullopt;
        std::optional<SkillEffectAttackProbabilityCondition> effectAttackProbabilityCondition = std::nullopt;

        static auto CreateFrom(const sox::SkillBasic& skillBasic) -> PlayerSkillData;
    };
}

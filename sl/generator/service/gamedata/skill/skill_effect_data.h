#pragma once
#include "sl/generator/service/gamedata/item/weapon_class_type.h"
#include "sl/generator/service/gamedata/skill/skill_effect_category.h"

namespace sunlight
{
    struct SkillEffectData
    {
        SkillEffectCategory category = SkillEffectCategory::None;
        int32_t type = 0;
        int32_t value1 = 0;
        int32_t value2 = 0;
        int32_t value3 = 0;
        int32_t value4 = 0;
        int32_t value5 = 0;
        int32_t value6 = 0;
        int32_t value7 = 0;
        int32_t value8 = 0;
        int32_t value9 = 0;
        int32_t value10 = 0;
        int32_t value11 = 0;
        int32_t value12 = 0;
        int32_t hopType = 0;
        int32_t hopA = 0;
        int32_t hopB = 0;
    };

    struct SkillEffectWeaponCondition
    {
        static constexpr auto CATEGORY = SkillEffectCategory::WeaponCondition;

        WeaponClassType weaponClass = WeaponClassType::None;
    };

    struct SkillEffectRangeWeaponCondition
    {
        static constexpr auto CATEGORY = SkillEffectCategory::WeaponCondition;
    };

    struct SkillEffectAttackProbabilityCondition
    {
        static constexpr auto CATEGORY = SkillEffectCategory::WeaponCondition;

        int32_t probabilityPerLevel = 0; //value2
        int32_t baseProbability = 0; // value3
    };
}

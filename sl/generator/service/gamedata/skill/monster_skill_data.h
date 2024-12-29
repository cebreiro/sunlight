#pragma once
#include <boost/container/static_vector.hpp>
#include "sl/generator/service/gamedata/skill/skill_effect_data.h"
#include "sl/generator/service/gamedata/skill/skill_target_area_type.h"
#include "sl/generator/service/gamedata/skill/skill_target_select_type.h"

namespace sunlight::sox
{
    struct MonsterSkill;
}

namespace sunlight
{
    struct AbilityValue;
}

namespace sunlight
{
    struct MonsterSkillData
    {
        int32_t index = 0;
        int32_t routineId = 0;
        int32_t abilityType = 0;
        bool passive = false;
        SkillTargetSelectType applyTargetType = {};
        SkillTargetingAreaType applyDamageType = {};
        int32_t damageLength = 0;
        int32_t damageLength2 = 0;
        int32_t damageMaxCount = 0;

        boost::container::static_vector<SkillEffectData, 4> effects;
        boost::container::static_vector<PtrNotNull<const AbilityValue>, 4> effectAttackValues;

        static auto CreateFrom(const sox::MonsterSkill& data) -> MonsterSkillData;
    };
}

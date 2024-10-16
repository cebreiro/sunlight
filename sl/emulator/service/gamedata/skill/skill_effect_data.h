#pragma once

namespace sunlight
{
    // category
    // 1: type -> damage?
    // 2: type -> status effect
    // 3: type -> stat
    // 4: type -> weaponClassType, restriction
    // 5: type -> summon
    // 6: type -> special skill passive effect
    struct SkillEffectData
    {
        int32_t category = 0;
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
}

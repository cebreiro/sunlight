#pragma once

namespace sunlight
{
    ENUM_CLASS(SkillEffectCategory, int32_t,
        (None, 0)
        (Damage, 1)
        (StatusEffect, 2)
        (Stat, 3) // client 0x4B177E
        (WeaponCondition, 4)
        (Summon, 5)
        (JobEffect, 6) // client 0x4B1787
    )
}

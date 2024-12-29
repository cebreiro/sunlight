#pragma once

namespace sunlight
{
    ENUM_CLASS(SkillEffectDamageType, int32_t,
        (Physical, 0)
        (Magic_Ice, 1)
        (Magic_Fire, 2)
        (Magic_Lightning, 3)
        (Magic_Arcane, 4)
    )

    inline bool IsMagicAttack(SkillEffectDamageType type)
    {
        switch (type)
        {
        case SkillEffectDamageType::Magic_Ice:
        case SkillEffectDamageType::Magic_Fire:
        case SkillEffectDamageType::Magic_Lightning:
        case SkillEffectDamageType::Magic_Arcane:
            return true;
        }

        return false;
    }
}

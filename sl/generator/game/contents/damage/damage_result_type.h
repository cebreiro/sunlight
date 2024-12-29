#pragma once

namespace sunlight
{
    ENUM_CLASS(DamageType, int32_t,
        (None, 0)
        (DamageMonster, 1)
        (DodgeMonster, 2)
        (Dead, 3)
        (DamagePlayer, 4)
        (DodgePlayer, 5)
        (Critical, 6)
    )

    ENUM_CLASS(DamageResultType, int32_t,
        (Damage_A, 0)
        (Damage_B, 1) // swirl
        (Damage_C, 2) // airborne
        (Damage_D, 3) // airborne + swirl
    )

    ENUM_CLASS(DamageBlowType, int32_t,
        (BlowSmall, 0) // monster default
        (BlowMiddle, 1)
        (BlowLarge, 2)
        (Unk3ForPlayer, 3)
    )
}

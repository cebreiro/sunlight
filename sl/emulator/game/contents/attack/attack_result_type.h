#pragma once

namespace sunlight
{
    // client 0x4963FF, 0x4A67EB, 0x493278
    ENUM_CLASS(AttackDamageType, int32_t,
        (None, 0)
        (DamageMonster, 1)
        (DodgeMonster, 2)
        (Dead, 3)
        (DamagePlayer, 4)
        (DodgePlayer, 5)
        (Critical, 6)
    )

    ENUM_CLASS(AttackedResultType, int32_t,
        (Damage_A, 0)
        (Damage_B, 1) // swirl
        (Damage_C, 2) // airborne
        (Damage_D, 3) // airborne + swirl
    )

    // client 0x49181C
    ENUM_CLASS(AttackTargetBlowType, int32_t,

        // player: 0x70000397  TARGETBLOW_NORMAL_SIZES.PNX
        // monster: 0x70000498 TARGETBLOW_NORMAL_SIZES0LED.PNX
        (BlowSmall, 0) // monster default

        // player: 0x70000396  TARGETBLOW_NORMAL_SIZEM.PNX
        // monster: 0x70000497 TARGETBLOW_NORMAL_SIZEM0LED.PNX
        (BlowMiddle, 1)

        // player: 0x70000395  TARGETBLOW_NORMAL_SIZEL.PNX
        // monster: 0x70000496 TARGETBLOW_NORMAL_SIZEL0LED.PNX
        (BlowLarge, 2)

        (Unk3ForPlayer, 3)
    )
}

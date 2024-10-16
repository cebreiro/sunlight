#pragma once

namespace sunlight
{
    // client 0x4B3884
    ENUM_CLASS(StatusEffectType, int32_t,
        (Heal, 0)
        (Resurrection, 1)
        (CrowdControlRemove, 2)
        (DyingResistance, 3)
        (StatIncrease, 4)
        (StatDecrease, 5)
        (ShieldFire, 6)
        (ShieldLighting, 7)
        (ShieldIce, 8)
        (ImmunePhysicalAttack, 10)
        (GuaranteedHitNeighbor, 12)
        (GuaranteedDodge, 13)
        (GuaranteedCritical, 14)
        (ReduceCastingTimeAndSP, 15)
        (Poison, 16)
        (Taunt, 18)
        (CrowdControlSleep, 20)
        (CrowdControlBinding, 21)
        (ReduceAggro, 22)
        (ReduceMonsterSight, 23)
        (RestSPBonus, 30)
    )
}

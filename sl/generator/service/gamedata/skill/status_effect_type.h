#pragma once

namespace sunlight
{
    ENUM_CLASS(StatusEffectType, int32_t,
        (Heal, 0)
        (Resurrection, 1)
        (CrowdControlRemove, 2)
        (DyingResistance, 3)
        (StatIncrease, 4)
        (StatDecrease, 5)
        (ShieldFire, 6) // damage reflect   -> player_skill 33, monster skill 200009
        (ShieldLighting, 7) // stun reflect -> monster skill 200011
        (ShieldIce, 8) // unk
        (ImmunePhysicalAttack, 10)
        (GuaranteedHitNeighbor, 12)
        (GuaranteedDodge, 13)
        (GuaranteedCritical, 14)
        (ReduceCastingTimeAndSP, 15)
        (Poison, 16)
        (CrowdControlParalysis, 18)
        (CrowdControlSleep, 20)
        (CrowdControlBinding, 21)
        (ReduceAggro, 22)
        (ReduceMonsterSight, 23)
        (RestSPBonus, 30)
    )
}

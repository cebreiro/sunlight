#pragma once

namespace sunlight
{
    ENUM_CLASS(JobId, int32_t,
        (None, 0)
        (Any, 900) /* for mix skill */

        (NoviceFighter, 1100)
        (SwordWarrior, 2100)
        (Berserker, 2101)
        (Dragoon, 2102)
        (MartialArtist, 2103)

        (NoviceRanger, 1201)
        (Archer, 2208)
        (Gunner, 2209)
        (Agent, 2210)
        (TreasureHunter, 2211)

        (NoviceMagician, 1302)
        (Mage, 2304)
        (Healer, 2305)
        (Mystic, 2306)
        (Engineer, 2307)

        (NoviceArtisan, 1403)
        (WeaponSmith, 2412)
        (Designer, 2413)
        (Cook, 2414)
        (Chemist, 2415)
    )
}

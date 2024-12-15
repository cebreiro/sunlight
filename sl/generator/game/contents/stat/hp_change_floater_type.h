#pragma once

namespace sunlight
{
    // client 0x49294B
    ENUM_CLASS(HPChangeFloaterType, int32_t,
        (Heal, 0)
        (None, 1)
        (PoisonDamage, 3)
        (DamagedByMonster, 4)
    )
}

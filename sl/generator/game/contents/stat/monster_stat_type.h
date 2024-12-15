#pragma once

namespace sunlight
{
    ENUM_CLASS(MonsterStatType, int32_t,
        (HitRate, 2) // client 4B4B40h
        (PhysicalDefense, 3) // client 4B4B90h
        (Attack, 10)
        (Fire, 11) // client 4B4A40h
        (Water, 12) // client 4B4A40h
        (Lightning, 13) // client 4B4A40h
    )
}

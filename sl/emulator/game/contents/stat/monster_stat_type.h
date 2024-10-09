#pragma once

namespace sunlight
{
    ENUM_CLASS(MonsterStatType, int32_t,
        (HitRate, 2) // client 4B4B40h
        (PhysicalDefense, 3) // client 4B4B90h
        (Str, 4)
        (Dex, 5)
        (Accr, 6)
        (Health, 7)
        (Intell, 8)
        (Wisdom, 9)
        (Will, 10)
        (Fire, 11) // client 4B4A40h
        (Water, 12) // client 4B4A40h
        (Lightning, 13) // client 4B4A40h
    )
}

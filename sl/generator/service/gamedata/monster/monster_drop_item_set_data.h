#pragma once

namespace sunlight
{
    class ItemData;
}

namespace sunlight
{
    struct MonsterDropItemSetData
    {
        int32_t id = 0;

        std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>> list;
    };
}

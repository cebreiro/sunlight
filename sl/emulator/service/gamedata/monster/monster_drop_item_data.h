#pragma once

namespace sunlight::sox
{
    struct MonsterItem;
}

namespace sunlight
{
    class ItemData;
    class ItemDataProvider;

    struct MonsterDropItemSetData;
}

namespace sunlight
{
    struct MonsterDropItemData
    {
        MonsterDropItemData() = default;
        MonsterDropItemData(const sox::MonsterItem& data, const ItemDataProvider& itemDataProvider,
            const std::unordered_map<int32_t, MonsterDropItemSetData>& itemSetTable);

        int32_t itemGenFrequency = 0;
        int32_t itemSetGenFrequency = 0;
        int32_t rareItemGenFrequency = 0;
        int32_t rareItemSetGenFrequency = 0;

        int32_t goldMod = 0;
        int32_t goldMin = 0;
        int32_t goldMax = 0;

        std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>> items;
        std::vector<std::pair<PtrNotNull<const ItemData>, int32_t>> rareItems;
        std::vector<std::pair<PtrNotNull<const MonsterDropItemSetData>, int32_t>> itemSets;
        std::vector<std::pair<PtrNotNull<const MonsterDropItemSetData>, int32_t>> rareItemSets;
    };
}

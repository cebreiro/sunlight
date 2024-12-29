#pragma once

namespace sunlight
{
    class ItemData;
}

namespace sunlight
{
    struct NPCShopItemBucket
    {
        int32_t width = 0;
        int32_t height = 0;

        std::vector<PtrNotNull<const ItemData>> items;
    };
}

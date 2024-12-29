#pragma once

namespace sunlight
{
    class ItemMixPropItemMapping
    {
        ItemMixPropItemMapping();

    public:
        static auto FindItemId(int32_t unk1, int32_t unk2) -> int32_t;

    private:
        std::unordered_map<uint32_t, int32_t> _items;

        static const ItemMixPropItemMapping _instance;

    };
}

#pragma once
#include "sl/generator/service/gamedata/shop/hair_shop_data.h"
#include "sl/generator/service/gamedata/shop/item_shop_data.h"

namespace sunlight
{
    class SoxTableSet;
}

namespace sunlight
{
    class NPCShopDataProvider
    {
    public:
        explicit NPCShopDataProvider(const SoxTableSet& tableSet);

        auto FindHairShopData(int32_t id) const -> const HairShopData*;
        auto FindItemShopData(int32_t id) const -> const ItemShopData*;

    private:
        std::unordered_map<int32_t, HairShopData> _hairShopData;
        std::unordered_map<int32_t, ItemShopData> _itemShopData;
    };
}

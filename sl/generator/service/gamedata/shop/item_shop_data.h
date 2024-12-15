#pragma once

namespace sunlight
{
    struct ItemShopData
    {
        int32_t id = 0;
        std::string_view name;
        float buyingFactor = 1.f;
        float sellingFactor = 1.f;

        std::vector<int32_t> saleItems;
    };
}

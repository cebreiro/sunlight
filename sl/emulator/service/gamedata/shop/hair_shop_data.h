#pragma once

namespace sunlight
{
    struct HairData
    {
        int32_t hair = 0;
        bool male = false;
        int32_t price = 0;
    };

    struct HairShopData
    {
        std::vector<HairData> hairs;

        int32_t coloringPrice = 0;
    };
}

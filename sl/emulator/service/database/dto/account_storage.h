#pragma once

namespace sunlight::db::dto
{
    struct AccountStorage
    {
        struct Item
        {
            int64_t id = 0;
            int32_t dataId = 0;
            int32_t quantity = 0;
            int8_t page = 0;
            int8_t x = 0;
            int8_t y = 0;
        };

        int64_t aid = 0;
        int8_t page = 1;
        int32_t gold = 0;

        std::vector<Item> items;
    };
}

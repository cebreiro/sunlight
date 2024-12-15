#pragma once
#include "sl/generator/service/database/enum/item_pos_type.h"

namespace sunlight::req
{
    struct ItemCreate
    {
        int64_t id = 0;
        int64_t cid = 0;
        int32_t itemId = 0;
        int32_t quantity = 0;

        db::ItemPosType posType = {};
        int8_t page = -1;
        int8_t x = -1;
        int8_t y = -1;

        friend void to_json(nlohmann::json& j, const ItemCreate& req);
    };
}

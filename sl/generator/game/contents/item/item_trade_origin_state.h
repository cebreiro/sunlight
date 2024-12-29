#pragma once
#include "sl/generator/game/contents/item/item_position.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"

namespace sunlight
{
    class ItemData;
}

namespace sunlight
{
    struct ItemTradeOriginState
    {
        game_entity_id_type id = {};
        GameEntityType type = {};
        PtrNotNull<const ItemData> data = nullptr;
        int32_t quantity = 0;
        ItemPositionType positionType = {};
        int8_t page = -1;
        int8_t x = -1;
        int8_t y = -1;
    };
}

#pragma once
#include "sl/generator/game/contents/item/equipment_position.h"

namespace sunlight
{
    struct ItemPositionUpdatePicked
    {
    };

    struct ItemPositionUpdateInventory
    {
        int8_t page = 0;
        int8_t x = 0;
        int8_t y = 0;
    };

    struct ItemPositionUpdateQuickSlot
    {
        int8_t page = 0;
        int8_t x = 0;
        int8_t y = 0;
    };

    struct ItemPositionUpdateEquipment
    {
        EquipmentPosition position = {};
    };

    struct ItemPositionUpdateVendor
    {
        int8_t page = 0;
    };

    struct ItemPositionUpdateMix
    {
    };

    using item_position_update_type = std::variant<ItemPositionUpdatePicked, ItemPositionUpdateInventory,
        ItemPositionUpdateQuickSlot, ItemPositionUpdateEquipment, ItemPositionUpdateVendor, ItemPositionUpdateMix>;
}

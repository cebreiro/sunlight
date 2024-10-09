#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"

namespace sunlight
{
    struct ItemRemoveResultDecrease
    {
        game_entity_id_type itemId = {};
        GameEntityType itemType = GameEntityType::None;

        int32_t decreaseQuantity = 0;
    };

    struct ItemRemoveResultRemove
    {
        game_entity_id_type itemId = {};
        GameEntityType itemType = GameEntityType::None;
    };

    using item_remove_result_type = std::variant<ItemRemoveResultDecrease, ItemRemoveResultRemove>;
}

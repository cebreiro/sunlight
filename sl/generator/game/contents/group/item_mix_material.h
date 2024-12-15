#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    struct ItemMixMaterial
    {
        game_entity_id_type itemId;
        int32_t quantity = 0;
    };
}

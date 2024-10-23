#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    struct EventBubblingMonsterAIAttach
    {
        game_entity_id_type entityId;
    };

    struct EventBubblingMonsterAIDetach
    {
        game_entity_id_type entityId;
    };
}

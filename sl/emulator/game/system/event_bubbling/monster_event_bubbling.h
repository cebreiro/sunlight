#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class GameEntity;
}

namespace sunlight
{
    struct EventBubblingMonsterSpawn
    {
        game_entity_id_type entityId;
    };

    struct EventBubblingMonsterDespawn
    {
        PtrNotNull<GameEntity> monster = nullptr;
    };
}

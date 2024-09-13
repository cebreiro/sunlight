#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"

namespace sunlight
{
    class GameEntityIdPool
    {
    public:
        static constexpr game_entity_id_type::value_type NULL_ID = 0;

    public:
        auto Pop(GameEntityType type) -> game_entity_id_type;
        auto Push(GameEntityType type, game_entity_id_type id);

    private:
        game_entity_id_type::value_type _nextValue = {};
        std::queue<game_entity_id_type> _recycleQueue;
    };
}

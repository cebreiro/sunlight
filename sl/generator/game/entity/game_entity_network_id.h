#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"

namespace sunlight
{
    class GameEntity;
}

namespace sunlight
{
    class GameEntityNetworkId
    {
    public:
        GameEntityNetworkId() = default;
        explicit GameEntityNetworkId(const GameEntity& entity);

        GameEntityNetworkId(game_entity_id_type id, GameEntityType type);

        auto ToBuffer() -> std::span<const char>;

        static auto Null() -> GameEntityNetworkId;

    private:
        std::array<char, 8> _array = {};
    };
}

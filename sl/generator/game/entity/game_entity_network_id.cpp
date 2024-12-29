#include "game_entity_network_id.h"

#include "sl/generator/game/entity/game_entity.h"

namespace sunlight
{
    GameEntityNetworkId::GameEntityNetworkId(const GameEntity& entity)
        : GameEntityNetworkId(entity.GetId(), entity.GetType())
    {
    }

    GameEntityNetworkId::GameEntityNetworkId(game_entity_id_type id, GameEntityType type)
    {
        *reinterpret_cast<uint32_t*>(_array.data()) = id.Unwrap();
        *reinterpret_cast<uint32_t*>(_array.data() + 4) = static_cast<uint32_t>(type);
    }

    auto GameEntityNetworkId::ToBuffer() -> std::span<const char>
    {
        return _array;
    }

    auto GameEntityNetworkId::Null() -> GameEntityNetworkId
    {
        return GameEntityNetworkId();
    }
}

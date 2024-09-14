#include "game_entity_network_id.h"

#include "sl/emulator/game/entity/game_entity.h"

namespace sunlight
{
    GameEntityNetworkId::GameEntityNetworkId(const GameEntity& entity)
    {
        *reinterpret_cast<uint32_t*>(_array.data()) = entity.GetId().Unwrap();
        *reinterpret_cast<uint32_t*>(_array.data() + 4) = static_cast<uint32_t>(entity.GetType());
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

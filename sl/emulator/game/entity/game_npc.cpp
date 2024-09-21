#include "game_npc.h"

namespace sunlight
{
    GameNPC::GameNPC(game_entity_id_type id, int32_t pnx)
        : GameEntity(id, TYPE)
        , _pnx(pnx)
    {
    }

    auto GameNPC::GetPNX() const -> int32_t
    {
        return _pnx;
    }

    auto GameNPC::GetName() const -> const std::string&
    {
        return _name;
    }

    void GameNPC::SetName(std::string name)
    {
        _name = std::move(name);
    }
}

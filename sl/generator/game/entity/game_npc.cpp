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

    auto GameNPC::GetUnk1() const -> int32_t
    {
        return _unk1;
    }

    auto GameNPC::GetUnk2() const -> int32_t
    {
        return _unk2;
    }

    void GameNPC::SetName(std::string name)
    {
        _name = std::move(name);
    }

    void GameNPC::SetUnk1(int32_t value)
    {
        _unk1 = value;
    }

    void GameNPC::SetUnk2(int32_t value)
    {
        _unk2 = value;
    }
}

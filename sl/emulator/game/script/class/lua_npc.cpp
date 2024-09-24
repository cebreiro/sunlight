#include "lua_npc.h"

#include <sol/state.hpp>

#include "sl/emulator/game/entity/game_npc.h"

namespace sunlight
{
    LuaNPC::LuaNPC(GameNPC& npc)
        : _npc(npc)
    {
    }

    void LuaNPC::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaNPC>("NPC",
            sol::no_constructor
        );
    }

    auto LuaNPC::GetId() const -> int32_t
    {
        return static_cast<int32_t>(_npc.GetId().Unwrap());
    }

    auto LuaNPC::GetImpl() const -> GameNPC&
    {
        return _npc;
    }
}

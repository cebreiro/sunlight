#include "lua_npc.h"

#include <sol/state.hpp>

#include "sl/generator/game/entity/game_npc.h"

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

    auto LuaNPC::GetId() const -> uint32_t
    {
        return _npc.GetId().Unwrap();
    }

    auto LuaNPC::GetImpl() const -> GameNPC&
    {
        return _npc;
    }
}

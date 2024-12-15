#include "lua_monster.h"

#include <sol/state.hpp>

#include "sl/generator/game/entity/game_monster.h"

namespace sunlight
{
    LuaMonster::LuaMonster(GameMonster& monster)
        : _monster(&monster)
    {
    }

    auto LuaMonster::GetId() const -> int32_t
    {
        return static_cast<int32_t>(_monster->GetId().Unwrap());
    }

    auto LuaMonster::GetTypeValue() const -> int32_t
    {
        return static_cast<int32_t>(_monster->GetType());
    }

    void LuaMonster::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaMonster>("Monster",
            sol::no_constructor,
            "getId", &LuaMonster::GetId,
            "getTypeValue", &LuaMonster::GetTypeValue
        );
    }
}

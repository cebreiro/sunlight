#include "lua_system.h"

#include <sol/state.hpp>

#include "sl/emulator/game/system/player_state_system.h"
#include "sl/emulator/game/time/game_time_service.h"

namespace sunlight
{
    LuaSystem::LuaSystem(PlayerStateSystem& system)
        : _system(system)
    {
    }

    void LuaSystem::Print(const std::string& str)
    {
        SUNLIGHT_LOG_INFO(_system.GetServiceLocator(),
            fmt::format("[lua_script] {}", str));
    }

    auto LuaSystem::GetTimePoint() const -> int64_t
    {
        return std::chrono::time_point_cast<std::chrono::milliseconds>(GameTimeService::Now()).time_since_epoch().count();
    }

    void LuaSystem::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaSystem>("System",
            sol::no_constructor,
            "print", &LuaSystem::Print,
            "getTimePoint", &LuaSystem::GetTimePoint
        );
    }
}

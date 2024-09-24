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

    void LuaSystem::Debug(const std::string& str)
    {
        SUNLIGHT_LOG_DEBUG(_system.GetServiceLocator(),
            fmt::format("[lua_script] {}", str));
    }

    void LuaSystem::Info(const std::string& str)
    {
        SUNLIGHT_LOG_INFO(_system.GetServiceLocator(),
            fmt::format("[lua_script] {}", str));
    }

    void LuaSystem::Warn(const std::string& str)
    {
        SUNLIGHT_LOG_WARN(_system.GetServiceLocator(),
            fmt::format("[lua_script] {}", str));
    }

    void LuaSystem::Error(const std::string& str)
    {
        SUNLIGHT_LOG_ERROR(_system.GetServiceLocator(),
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
            "debug", &LuaSystem::Debug,
            "info", &LuaSystem::Info,
            "warn", &LuaSystem::Warn,
            "error", &LuaSystem::Error,
            "getTimePoint", &LuaSystem::GetTimePoint
        );
    }
}

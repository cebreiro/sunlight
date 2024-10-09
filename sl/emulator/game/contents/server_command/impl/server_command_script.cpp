#include "server_command_script.h"

#include "sl/emulator/game/script/lua_script_engine.h"
#include "sl/emulator/game/script/class/lua_player.h"
#include "sl/emulator/game/system/player_state_system.h"
#include "sl/emulator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandScriptReload::ServerCommandScriptReload(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandScriptReload::GetName() const -> std::string_view
    {
        return "script_reload";
    }

    auto ServerCommandScriptReload::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandScriptReload::Execute(GamePlayer& player) const
    {
        (void)player;

        return _system.GetServiceLocator().Get<LuaScriptEngine>().Reload();
    }

    ServerCommandScriptExecute::ServerCommandScriptExecute(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandScriptExecute::GetName() const -> std::string_view
    {
        return "script_execute";
    }

    auto ServerCommandScriptExecute::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandScriptExecute::Execute(GamePlayer& player, std::string name) const
    {
        LuaScriptEngine& luaScriptEngine = _system.GetServiceLocator().Get<LuaScriptEngine>();
        if (!luaScriptEngine.ReloadCommandScript())
        {
            return false;
        }

        LuaPlayer luaPlayer(_system.Get<PlayerStateSystem>(), player);

        return luaScriptEngine.ExecuteCommandScript(name, luaPlayer);
    }
}

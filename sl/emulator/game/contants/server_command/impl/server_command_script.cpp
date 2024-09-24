#include "server_command_script.h"

#include "sl/emulator/game/script/lua_script_engine.h"
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
}

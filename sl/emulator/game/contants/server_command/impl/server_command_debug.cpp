#include "server_command_debug.h"

#include "sl/emulator/game/debug/game_debugger.h"
#include "sl/emulator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandDebugServer::ServerCommandDebugServer(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugServer::GetName() const -> std::string_view
    {
        return "debug_server";
    }

    auto ServerCommandDebugServer::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugServer::Execute(GamePlayer& player, int32_t type, int32_t value) const
    {
        (void)player;

        if (GameDebugger* debugger = _system.GetServiceLocator().Find<GameDebugger>(); debugger)
        {
            if (type < 0 || type >= static_cast<int32_t>(GameDebugType::Count))
            {
                return false;
            }

            debugger->SetState(static_cast<GameDebugType>(type), value);

            return true;
        }

        return false;
    }

    ServerCommandDebugServerOff::ServerCommandDebugServerOff(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugServerOff::GetName() const -> std::string_view
    {
        return "debug_server_off";
    }

    auto ServerCommandDebugServerOff::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugServerOff::Execute(GamePlayer& player) const
    {
        (void)player;

        if (GameDebugger* debugger = _system.GetServiceLocator().Find<GameDebugger>(); debugger)
        {
            debugger->Clear();

            return true;
        }

        return false;
    }
}

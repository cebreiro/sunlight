#include "server_command_register.h"

#include "sl/emulator/game/contants/server_command/server_command.hpp"
#include "sl/emulator/game/system/server_command_system.h"

namespace sunlight
{
    void ServerCommandRegister::Register(ServerCommandSystem& system)
    {
        system.AddCommand(std::make_shared<ServerCommandItemAdd>(system));
        system.AddCommand(std::make_shared<ServerCommandItemGain>(system));
        system.AddCommand(std::make_shared<ServerCommandItemSpawn>(system));

        system.AddCommand(std::make_shared<ServerCommandShowStringTable>());
    }
}

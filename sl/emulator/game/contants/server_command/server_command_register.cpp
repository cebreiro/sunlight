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

        system.AddCommand(std::make_shared<ServerCommandPacketObjectLeave>());
        system.AddCommand(std::make_shared<ServerCommandPacketHairColorChange>(system));
        system.AddCommand(std::make_shared<ServerCommandPacketSkinColorChange>(system));

        system.AddCommand(std::make_shared<ServerCommandExpGain>(system));
        system.AddCommand(std::make_shared<ServerCommandExpCharacterGain>(system));
        system.AddCommand(std::make_shared<ServerCommandExpJobGain>(system));

        system.AddCommand(std::make_shared<ServerCommandDebugServer>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugServerOff>(system));

        system.AddCommand(std::make_shared<ServerCommandJobPromote>(system));

        system.AddCommand(std::make_shared<ServerCommandTeleport>(system));
    }
}

#include "server_command_register.h"

#include "sl/generator/game/contents/server_command/server_command.hpp"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    void ServerCommandRegister::Register(ServerCommandSystem& system)
    {
        system.AddCommand(std::make_shared<ServerCommandItemAdd>(system));
        system.AddCommand(std::make_shared<ServerCommandItemSpawn>(system));

        system.AddCommand(std::make_shared<ServerCommandPacketObjectLeave>());
        system.AddCommand(std::make_shared<ServerCommandPacketHairColorChange>(system));
        system.AddCommand(std::make_shared<ServerCommandPacketSkinColorChange>(system));
        system.AddCommand(std::make_shared<ServerCommandPacketTest>());

        system.AddCommand(std::make_shared<ServerCommandExpGain>(system));
        system.AddCommand(std::make_shared<ServerCommandExpCharacterGain>(system));
        system.AddCommand(std::make_shared<ServerCommandExpJobGain>(system));

        system.AddCommand(std::make_shared<ServerCommandDebugServer>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugServerOff>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugPlayerHP>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugPlayerSP>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugTile>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugPathFind>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugPathFindRaw>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugPathBlocked>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugMonsterPosition>(system));
        system.AddCommand(std::make_shared<ServerCommandDebugStringTable>());

        system.AddCommand(std::make_shared<ServerCommandJobPromote>(system));

        system.AddCommand(std::make_shared<ServerCommandTeleport>(system));

        system.AddCommand(std::make_shared<ServerCommandScriptReload>(system));
        system.AddCommand(std::make_shared<ServerCommandScriptExecute>(system));

        system.AddCommand(std::make_shared<ServerCommandNPCShopRoll>(system));
        system.AddCommand(std::make_shared<ServerCommandNPCShopDebugSlot>(system));

        system.AddCommand(std::make_shared<ServerCommandStageChange>(system));

        system.AddCommand(std::make_shared<ServerCommandZoneChange>(system));

        system.AddCommand(std::make_shared<ServerCommandSkillAdd>(system));

        system.AddCommand(std::make_shared<ServerCommandStatHPSet>(system));
        system.AddCommand(std::make_shared<ServerCommandStatSPSet>(system));
        system.AddCommand(std::make_shared<ServerCommandStatShow>());

        system.AddCommand(std::make_shared<ServerCommandMonsterSpawn>(system));
        system.AddCommand(std::make_shared<ServerCommandMonsterKill>(system));
        system.AddCommand(std::make_shared<ServerCommandMonsterRemove>(system));
        system.AddCommand(std::make_shared<ServerCommandMonsterAISuspend>(system));
        system.AddCommand(std::make_shared<ServerCommandMonsterAIResume>(system));
        system.AddCommand(std::make_shared<ServerCommandMonsterComeHere>(system));

        system.AddCommand(std::make_shared<ServerCommandDamageSet>());
        system.AddCommand(std::make_shared<ServerCommandDamageClear>());

        system.AddCommand(std::make_shared<ServerCommandQuestAdd>(system));
        system.AddCommand(std::make_shared<ServerCommandQuestStateSet>(system));
        system.AddCommand(std::make_shared<ServerCommandQuestFlagSet>(system));

        system.AddCommand(std::make_shared<ServerCommandSuicide>(system));
        system.AddCommand(std::make_shared<ServerCommandGoldAdd>(system));

        system.AddCommand(std::make_shared<ServerCommandQueryNesCaller>(system));

        system.AddCommand(std::make_shared<ServerCommandStatusEffectPlayerAdd>(system));
    }
}

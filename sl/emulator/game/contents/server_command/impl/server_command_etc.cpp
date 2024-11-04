#include "server_command_etc.h"

#include "sl/emulator/game/system/entity_damage_system.h"
#include "sl/emulator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandSuicide::ServerCommandSuicide(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandSuicide::GetName() const -> std::string_view
    {
        return "suicide";
    }

    auto ServerCommandSuicide::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandSuicide::Execute(GamePlayer& player) const
    {
        _system.Get<EntityDamageSystem>().KillPlayer(player);

        return true;
    }
}

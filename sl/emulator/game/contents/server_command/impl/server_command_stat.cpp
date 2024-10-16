#include "server_command_stat.h"

#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandStatHPSet::ServerCommandStatHPSet(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandStatHPSet::GetName() const -> std::string_view
    {
        return "stat_hp_set";
    }

    auto ServerCommandStatHPSet::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandStatHPSet::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<PlayerStatSystem>().SetHP(player, value, HPChangeFloaterType::None);

        return true;
    }

    ServerCommandStatSPSet::ServerCommandStatSPSet(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandStatSPSet::GetName() const -> std::string_view
    {
        return "stat_sp_set";
    }

    auto ServerCommandStatSPSet::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandStatSPSet::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<PlayerStatSystem>().SetSP(player, value, SPChangeFloaterType::None);

        return true;
    }
}

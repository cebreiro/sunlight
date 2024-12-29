#include "server_command_exp.h"

#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandExpGain::ServerCommandExpGain(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandExpGain::GetName() const -> std::string_view
    {
        return "exp_gain";
    }

    auto ServerCommandExpGain::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandExpGain::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<PlayerStatSystem>().GainCharacterExp(player, value);
        _system.Get<PlayerJobSystem>().GainJobExp(player, value);

        return true;
    }

    ServerCommandExpCharacterGain::ServerCommandExpCharacterGain(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandExpCharacterGain::GetName() const -> std::string_view
    {
        return "exp_character_gain";
    }

    auto ServerCommandExpCharacterGain::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandExpCharacterGain::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<PlayerStatSystem>().GainCharacterExp(player, value);

        return true;
    }

    ServerCommandExpJobGain::ServerCommandExpJobGain(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandExpJobGain::GetName() const -> std::string_view
    {
        return "exp_job_gain";
    }

    auto ServerCommandExpJobGain::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandExpJobGain::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<PlayerJobSystem>().GainJobExp(player, value);

        return true;
    }
}

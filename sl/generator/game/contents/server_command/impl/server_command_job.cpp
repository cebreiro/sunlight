#include "server_command_job.h"

#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandJobPromote::ServerCommandJobPromote(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandJobPromote::GetName() const -> std::string_view
    {
        return "job_promote";
    }

    auto ServerCommandJobPromote::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandJobPromote::Execute(GamePlayer& player, int32_t advanced) const
    {
        return _system.Get<PlayerJobSystem>().Promote(player, static_cast<JobId>(advanced));
    }
}

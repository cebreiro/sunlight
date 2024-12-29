#include "server_command_stage.h"

#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/game/zone/service/zone_change_service.h"

namespace sunlight
{
    ServerCommandStageChange::ServerCommandStageChange(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandStageChange::GetName() const -> std::string_view
    {
        return "stage_change";
    }

    auto ServerCommandStageChange::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandStageChange::Execute(GamePlayer& player, int32_t destStageId, int32_t destX, int32_t destY) const
    {
        return _system.GetServiceLocator().Get<ZoneChangeService>().StartStageChange(player, destStageId, destX, destY);
    }
}

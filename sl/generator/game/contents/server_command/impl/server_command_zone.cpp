#include "server_command_zone.h"

#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/game/zone/service/zone_change_service.h"

namespace sunlight
{
    ServerCommandZoneChange::ServerCommandZoneChange(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandZoneChange::GetName() const -> std::string_view
    {
        return "zone_change";
    }

    auto ServerCommandZoneChange::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandZoneChange::Execute(GamePlayer& player, int32_t destZoneId, int32_t destX, int32_t destY) const
    {
        _system.GetServiceLocator().Get<ZoneChangeService>().StartZoneChange(player.GetClientId(), destZoneId, destX, destY);

        return true;
    }
}

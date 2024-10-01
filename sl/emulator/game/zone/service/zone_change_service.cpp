#include "zone_change_service.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/zone/zone.h"

namespace sunlight
{
    ZoneChangeService::ZoneChangeService(Zone& zone)
        : _zone(zone)
    {
    }

    auto ZoneChangeService::GetName() const -> std::string_view
    {
        return "zone_change_service";
    }

    bool ZoneChangeService::StartStageChange(GamePlayer& player, int32_t destStageId, int32_t destX, int32_t destY)
    {
        if (!_zone.FindStage(destStageId))
        {
            return false;
        }

        const game_client_id_type clientId = player.GetClientId();
        const int64_t cid = player.GetCId();

        Post(_zone.GetStrand(),
            [zone = _zone.shared_from_this(), clientId, cid, destStageId, destX, destY]()
            {
                zone->ChangePlayerStage(clientId, destStageId, destX, destY)
                    .Then(*ExecutionContext::GetExecutor(), [zone, cid, destStageId](bool success)
                        {
                            if (!success)
                            {
                                SUNLIGHT_LOG_WARN(zone->GetServiceLocator(),
                                    fmt::format("fail to change stage. player: {}, dest_stage: {}",
                                        cid, destStageId));
                            }
                        });
            });

        return true;
    }
}

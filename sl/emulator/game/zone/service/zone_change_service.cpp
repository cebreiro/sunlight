#include "zone_change_service.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/zone/zone.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/emulator/service/authentication/authentication_service.h"
#include "sl/emulator/service/gateway/gateway_service.h"

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

    auto ZoneChangeService::StartZoneChange(game_client_id_type clientId, int32_t destZoneId, int32_t destX, int32_t destY) -> Future<void>
    {
        [[maybe_unused]]
        const auto holder = _zone.shared_from_this();

        const ServiceLocator& serviceLocator = _zone.GetServiceLocator();

        const auto queryResult = co_await serviceLocator.Get<GatewayService>().FindZone(_zone.GetWorldId(), destZoneId);
        if (!queryResult.has_value())
        {
            co_return;
        }

        GameClient* clientPtr = _zone.FindClient(clientId);
        if (!clientPtr)
        {
            co_return;
        }

        SharedPtrNotNull<GameClient> client = clientPtr->shared_from_this();

        const bool removed = co_await _zone.RemovePlayerByZoneChange(clientId, destZoneId, (float)destX, (float)destY, 0.0);
        if (!removed)
        {
            co_return;
        }

        const std::shared_ptr<AuthenticationToken>& authToken = client->GetAuthenticationToken();
        if (!authToken)
        {
            assert(false);

            client->Disconnect();

            co_return;
        }

        client->SetState(GameClientState::ZoneChaning);
        client->Send(ServerType::Zone, ZonePacketS2CCreator::CreateZoneChange(
            queryResult->first, queryResult->second, authToken->GetKey().GetLow()));

        co_return;
    }
}

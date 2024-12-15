#include "zone_change_service.h"

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/event_object_stage_exit_portal_component.h"
#include "sl/generator/game/entity/game_event_object.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/event_object_system.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/zone.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/generator/service/authentication/authentication_service.h"
#include "sl/generator/service/gateway/gateway_service.h"

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

    void ZoneChangeService::RegisterStageExitPortal(int32_t linkId, int32_t stageId, game_entity_id_type entityId)
    {
        _stageExitPortalLinks.emplace(linkId, std::make_pair(stageId, entityId));
    }

    bool ZoneChangeService::StartStageChange(GamePlayer& player, int32_t currentStageId, int32_t linkId)
    {
        const auto [begin, end] = _stageExitPortalLinks.equal_range(linkId);
        if (begin == end)
        {
            return false;
        }

        const auto iter = std::ranges::find_if(begin, end, [currentStageId](const auto& pair) -> bool
            {
                return pair.first != currentStageId;
            });
        if (iter == end)
        {
            return false;
        }

        const auto& [stageId, eventObjectId] = iter->second;

        Stage* stage = _zone.FindStage(stageId);
        if (!stage)
        {
            return false;
        }

        const GameEventObject* eventObject = stage->Get<EventObjectSystem>().FindEventObject(eventObjectId);
        if (!eventObject)
        {
            return false;
        }

        const EventObjectStageExitPortalComponent* stageExitComponent = eventObject->FindComponent<EventObjectStageExitPortalComponent>();
        if (!stageExitComponent)
        {
            return false;
        }

        const Eigen::Vector2f& center = eventObject->GetCenterPosition();

        return StartStageChange(player, stageId, static_cast<int32_t>(center.x()), static_cast<int32_t>(center.y()), eventObject->GetYaw());
    }

    bool ZoneChangeService::StartStageChange(GamePlayer& player, int32_t destStageId, int32_t destX, int32_t destY, std::optional<float> yaw)
    {
        if (!_zone.FindStage(destStageId))
        {
            return false;
        }

        const game_client_id_type clientId = player.GetClientId();
        const int64_t cid = player.GetCId();

        Post(_zone.GetStrand(),
            [zone = _zone.shared_from_this(), clientId, cid, destStageId, destX, destY, yaw]()
            {
                zone->ChangePlayerStage(clientId, destStageId, destX, destY, yaw)
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

    auto ZoneChangeService::StartZoneChange(game_client_id_type clientId, int32_t destZoneId, int32_t destX, int32_t destY, std::optional<float> yaw) -> Future<void>
    {
        return StartZoneChange(clientId, destZoneId, GameConstant::STAGE_MAIN, destX, destY, yaw);
    }

    auto ZoneChangeService::StartZoneChange(game_client_id_type clientId, int32_t destZoneId, int32_t destStage, int32_t destX, int32_t destY, std::optional<float> yaw) -> Future<void>
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

        const bool removed = co_await _zone.RemovePlayerByZoneChange(clientId, destZoneId, destStage, (float)destX, (float)destY, yaw.value_or(0.f));
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

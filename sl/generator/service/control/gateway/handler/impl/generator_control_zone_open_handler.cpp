#include "generator_control_zone_open_handler.h"

#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlZoneOpenHandler::GeneratorControlZoneOpenHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlZoneOpenHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::ZoneOpenRequest& request) -> Future<void>
    {
        const int32_t worldId = request.world_id();
        const int32_t zoneId = request.zone_id();
        const uint16_t port = static_cast<uint16_t>(request.port());
        std::string errorMessage;

        const bool success = co_await _serviceLocator.Get<GeneratorControlService>().OpenZone(worldId, zoneId, port, &errorMessage);

        api::Response response;
        response.set_request_id(requestId);

        api::ZoneOpenResponse& zoneOpenResponse = *response.mutable_zone_open();
        zoneOpenResponse.set_success(success);

        if (!success)
        {
            zoneOpenResponse.set_error_message(errorMessage);
        }

        connection.Send(std::move(response));
    }
}

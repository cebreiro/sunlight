#include "generator_control_zone_close_handler.h"

#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlZoneCloseHandler::GeneratorControlZoneCloseHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlZoneCloseHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::ZoneCloseRequest& request) -> Future<void>
    {
        const int32_t worldId = request.world_id();
        const int32_t zoneId = request.zone_id();
        std::string errorMessage;

        const bool success = co_await _serviceLocator.Get<GeneratorControlService>().CloseZone(worldId, zoneId, &errorMessage);

        api::Response response;
        response.set_request_id(requestId);

        api::ZoneCloseResponse& zoneCloseResponse = *response.mutable_zone_close();
        zoneCloseResponse.set_success(success);

        if (!success)
        {
            zoneCloseResponse.set_error_message(errorMessage);
        }

        connection.Send(std::move(response));
    }
}

#include "generator_control_world_info_handler.h"

#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlWorldInfoHandler::GeneratorControlWorldInfoHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlWorldInfoHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::WorldInfoRequest& request) -> Future<void>
    {
        api::Response response;
        response.set_request_id(requestId);

        api::WorldInfoResponse& worldInfoResponse = *response.mutable_world_info();

        std::vector<api::WorldInfo> worldInfoList = co_await _serviceLocator.Get<GeneratorControlService>().GetWorldInfo();

        for (api::WorldInfo& worldInfo : worldInfoList)
        {
            worldInfoResponse.mutable_world_info_list()->Add(std::move(worldInfo));
        }

        connection.Send(std::move(response));

        co_return;
    }
}

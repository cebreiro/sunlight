#include "generator_control_user_count_handler.h"

#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlUserCountHandler::GeneratorControlUserCountHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlUserCountHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::UserCountRequest& request) -> Future<void>
    {
        const int32_t userCount = co_await _serviceLocator.Get<GeneratorControlService>().GetUserCount();

        api::Response response;
        response.set_request_id(requestId);

        api::UserCountResponse& userCountResponse = *response.mutable_user_count();
        userCountResponse.set_user_count(userCount);

        connection.Send(std::move(response));

        co_return;
    }
}

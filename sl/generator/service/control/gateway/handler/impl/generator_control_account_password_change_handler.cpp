#include "generator_control_account_password_change_handler.h"

#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlAccountPasswordChangeHandler::GeneratorControlAccountPasswordChangeHandler(
        const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlAccountPasswordChangeHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::AccountPasswordChangeRequest& request) -> Future<void>
    {
        std::string error;

        const bool success = co_await _serviceLocator.Get<GeneratorControlService>().ChangeAccountPassword(
            request.id(), request.password(), connection.GetLevel(), &error);

        api::Response response;
        response.set_request_id(requestId);

        api::AccountPasswordChangeResponse& accountPasswordChangeResponse = *response.mutable_account_password_change();
        accountPasswordChangeResponse.set_success(success);

        if (!error.empty())
        {
            accountPasswordChangeResponse.set_error_message(error);
        }

        connection.Send(std::move(response));

        co_return;
    }
}

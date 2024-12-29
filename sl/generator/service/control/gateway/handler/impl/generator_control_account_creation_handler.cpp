#include "generator_control_account_creation_handler.h"

#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlAccountCreationHandler::GeneratorControlAccountCreationHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlAccountCreationHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::AccountCreationReuqest& request) -> Future<void>
    {
        if (connection.GetState() != GeneratorControlAPIGatewayConnection::State::Authenticated)
        {
            throw std::runtime_error("unauthenticated request");
        }

        const int8_t gmLevel = static_cast<int8_t>(request.gm_level());

        std::string error;
        const bool success = co_await _serviceLocator.Get<GeneratorControlService>().CreateAccount(request.id(), request.password(), gmLevel, &error);

        api::Response response;
        response.set_request_id(requestId);

        api::AccountCreationResponse& creationResponse = *response.mutable_account_creation();
        creationResponse.set_success(success);

        if (!error.empty())
        {
            creationResponse.set_error_message(error);
        }

        connection.Send(std::move(response));

        co_return;
    }
}

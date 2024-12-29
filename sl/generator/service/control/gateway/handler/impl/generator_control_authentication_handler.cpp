#include "generator_control_authentication_handler.h"

#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlAuthenticationHandler::GeneratorControlAuthenticationHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlAuthenticationHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::AuthenticationRequest& request) -> Future<void>
    {
        std::optional<int32_t> result =
            co_await _serviceLocator.Get<GeneratorControlService>().Authenticate(request.id(), request.password());

        if (result.has_value())
        {
            connection.SetState(GeneratorControlAPIGatewayConnection::State::Authenticated);
            connection.SetLevel(static_cast<int8_t>(*result));
        }

        api::Response response;
        response.set_request_id(requestId);

        api::AuthenticationResponse* authentication = response.mutable_authentication();
        authentication->set_success(result.has_value());

        connection.Send(std::move(response));

        co_return;
    }
}

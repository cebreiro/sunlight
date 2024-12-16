#include "generator_control_authentication_handler.h"

#include "shared/network/session/session.h"
#include "sl/generator/api/generated/response.pb.h"
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
        SUNLIGHT_LOG_INFO(_serviceLocator,
            fmt::format("req: {}, {}, {}", requestId, request.id(), request.password()));

        api::Response response;
        response.set_request_id(requestId);

        api::AuthenticationResponse* authentication = response.mutable_authentication();
        authentication->set_success(false);

        const int32_t bodySize = static_cast<int32_t>(response.ByteSizeLong());

        buffer::Fragment body = buffer::Fragment::Create(bodySize);
        response.SerializeToArray(body.GetData(), bodySize);

        Buffer sendBuffer;
        sendBuffer.Add(buffer::Fragment::Create(4));
        sendBuffer.Add(std::move(body));

        BufferWriter writer(sendBuffer);
        writer.Write<int32_t>(bodySize + 4);

        connection.session->Send(std::move(sendBuffer));

        co_return;
    }
}

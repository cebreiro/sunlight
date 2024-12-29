#pragma once
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_handler_interface.h"

namespace sunlight
{
    class GeneratorControlAuthenticationHandler : public GeneratorControlRequestHandlerT<api::AuthenticationRequest>
    {
    public:
        explicit GeneratorControlAuthenticationHandler(const ServiceLocator& serviceLocator);

        auto HandleRequest(GeneratorControlAPIGatewayConnection& connection,
            int32_t requestId, const api::AuthenticationRequest& request) -> Future<void> override;

    private:
        const ServiceLocator& _serviceLocator;
    };
}

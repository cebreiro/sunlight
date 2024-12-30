#pragma once
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_handler_interface.h"

namespace sunlight
{
    class GeneratorControlLogGetHandler : public GeneratorControlRequestHandlerT<api::LogGetRequest>
    {
    public:
        explicit GeneratorControlLogGetHandler(const ServiceLocator& serviceLocator);

        auto HandleRequest(GeneratorControlAPIGatewayConnection& connection,
            int32_t requestId, const api::LogGetRequest& request) -> Future<void> override;

    private:
        const ServiceLocator& _serviceLocator;
    };
}

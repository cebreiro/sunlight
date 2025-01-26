#pragma once
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_handler_interface.h"

namespace sunlight
{
    class GeneratorControlZoneCloseHandler : public GeneratorControlRequestHandlerT<api::ZoneCloseRequest>
    {
    public:
        explicit GeneratorControlZoneCloseHandler(const ServiceLocator& serviceLocator);

        auto HandleRequest(GeneratorControlAPIGatewayConnection& connection,
            int32_t requestId, const api::ZoneCloseRequest& request) -> Future<void> override;

    private:
        const ServiceLocator& _serviceLocator;
    };
}

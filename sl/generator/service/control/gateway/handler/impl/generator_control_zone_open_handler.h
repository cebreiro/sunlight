#pragma once
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_handler_interface.h"

namespace sunlight
{
    class GeneratorControlZoneOpenHandler : public GeneratorControlRequestHandlerT<api::ZoneOpenRequest>
    {
    public:
        explicit GeneratorControlZoneOpenHandler(const ServiceLocator& serviceLocator);

        auto HandleRequest(GeneratorControlAPIGatewayConnection& connection,
            int32_t requestId, const api::ZoneOpenRequest& request) -> Future<void> override;

    private:
        const ServiceLocator& _serviceLocator;
    };
}

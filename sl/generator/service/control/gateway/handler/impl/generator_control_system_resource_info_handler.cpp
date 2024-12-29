#include "generator_control_system_resource_info_handler.h"

#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace sunlight
{
    GeneratorControlSystemResourceInfoHandler::GeneratorControlSystemResourceInfoHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlSystemResourceInfoHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::SystemResourceInfoRequest& request) -> Future<void>
    {
        std::optional<int32_t> cpuLoadPercentage = std::nullopt;
        std::optional<double> freeMemoryGB = std::nullopt;

        co_await _serviceLocator.Get<GeneratorControlService>().GetSystemResourceInfo(cpuLoadPercentage, freeMemoryGB);

        api::Response response;
        response.set_request_id(requestId);

        api::SystemResourceInfoResponse& systemInfo = *response.mutable_system_resource_info();
        systemInfo.set_cpu_load_percentage(cpuLoadPercentage.value_or(-1));
        systemInfo.set_free_memory_gb(freeMemoryGB.value_or(-1.0));

        connection.Send(std::move(response));
    }
}

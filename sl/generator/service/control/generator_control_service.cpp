#include "generator_control_service.h"

#include "sl/generator/service/control/gateway/generator_control_api_gateway_server.h"

namespace sunlight
{
    GeneratorControlService::GeneratorControlService(ServiceLocator& serviceLocator, execution::AsioExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _apiGatewayServer(std::make_shared<GeneratorControlAPIGatewayServer>(executor))
    {
    }

    void GeneratorControlService::Initialize()
    {
        _apiGatewayServer->Initialize(_serviceLocator);
    }

    void GeneratorControlService::StartGatewayServer(uint16_t port)
    {
        _apiGatewayServer->StartUp(port);
    }

    auto GeneratorControlService::Authenticate(std::string id, std::string password) -> Future<std::optional<int32_t>>
    {
        (void)id;
        (void)password;

        co_return std::optional<int32_t>();
    }

    auto GeneratorControlService::GetName() const -> std::string_view
    {
        return "generator_control_service";
    }
}

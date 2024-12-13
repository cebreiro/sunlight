#include "generator_control_service.h"

#include "sl/generator/service/control/gateway/generator_control_api_gateway_server.h"
#include "sl/generator/service/database/database_service.h"
#include "sl/generator/service/hash/safe_hash_service.h"

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
        std::optional<db::dto::Account> result = co_await _serviceLocator.Get<DatabaseService>().FindAccount(id);
        if (!result.has_value())
        {
            co_return std::nullopt;
        }

        co_return 1;

        /*if (!co_await _serviceLocator.Get<SafeHashService>().Compare(result->password, password))
        {
            co_return std::nullopt;
        }

        co_return result->gmLevel;*/
    }

    auto GeneratorControlService::GetName() const -> std::string_view
    {
        return "generator_control_service";
    }
}

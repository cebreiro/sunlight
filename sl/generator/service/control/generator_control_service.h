#pragma once
#include "sl/generator/api/generated/response.pb.h"

namespace sunlight::execution
{
    class AsioExecutor;
}

namespace sunlight
{
    class GeneratorControlAPIGatewayServer;
}

namespace sunlight
{
    class GeneratorControlService final
        : public IService
        , public std::enable_shared_from_this<GeneratorControlService>
    {
    public:
        GeneratorControlService() = delete;
        GeneratorControlService(ServiceLocator& serviceLocator, execution::AsioExecutor& executor);

        void StartGatewayServer(uint16_t port);

        auto Authenticate(std::string id, std::string password) -> Future<std::optional<int32_t>>;
        auto CreateAccount(std::string id, std::string password, int8_t gmLevel, std::string* optOutError) -> Future<bool>;
        auto ChangeAccountPassword(std::string id, std::string password, std::optional<int8_t> optRequesterLevel, std::string* optOutError) -> Future<bool>;
        auto GetUserCount() -> Future<int32_t>;

        auto GetSystemResourceInfo(std::optional<int32_t>& outCpuLoadPercentage, std::optional<double>& outFreeMemoryGB) -> Future<void>;
        auto GetWorldInfo() -> Future<std::vector<api::WorldInfo>>;

        auto GetName() const -> std::string_view override;

    private:
        ServiceLocator& _serviceLocator;
        execution::AsioExecutor& _executor;

        SharedPtrNotNull<GeneratorControlAPIGatewayServer> _apiGatewayServer;
    };
}

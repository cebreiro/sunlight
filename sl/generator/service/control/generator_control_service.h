#pragma once

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

        void Initialize();
        void StartGatewayServer(uint16_t port);

        auto Authenticate(std::string id, std::string password) -> Future<std::optional<int32_t>>;

        auto GetName() const -> std::string_view override;

    private:
        ServiceLocator& _serviceLocator;

        SharedPtrNotNull<GeneratorControlAPIGatewayServer> _apiGatewayServer;
    };
}

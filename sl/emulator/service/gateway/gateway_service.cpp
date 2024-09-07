#include "gateway_service.h"

namespace sunlight
{
    GatewayService::GatewayService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
    {
    }

    auto GatewayService::Run() -> Future<void>
    {
        co_return;
    }

    void GatewayService::Shutdown()
    {
    }

    void GatewayService::Join(boost::system::error_code& ec)
    {
        (void)ec;
    }

    auto GatewayService::GetName() const -> std::string_view
    {
        return "gateway_service";
    }

    auto GatewayService::AddLobby(int8_t serverId, std::string address) -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        _lobbies.emplace_back(serverId, std::move(address));

        co_return;
    }

    auto GatewayService::GetLobbies() const -> Future<std::vector<std::pair<int8_t, std::string>>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        co_return _lobbies;
    }
}

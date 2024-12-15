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

    auto GatewayService::AddZone(int8_t serverId, std::string address, uint16_t port, int32_t zoneId) -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        _zones[serverId][zoneId] = std::make_pair(std::move(address), port);

        co_return;
    }

    auto GatewayService::FindZone(int8_t serverId, int32_t zoneId) const -> Future<std::optional<std::pair<std::string, uint16_t>>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        const auto iter1 = _zones.find(serverId);
        if (iter1 == _zones.end())
        {
            co_return std::nullopt;
        }

        const auto iter2 = iter1->second.find(zoneId);
        if (iter2 == iter1->second.end())
        {
            co_return std::nullopt;
        }

        co_return iter2->second;
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

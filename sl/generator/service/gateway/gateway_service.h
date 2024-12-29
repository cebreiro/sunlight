#pragma once
#include "sl/generator/service/emulation_service_interface.h"

namespace sunlight
{
    class GatewayService final
        : public IEmulationService
        , public std::enable_shared_from_this<GatewayService>
    {
    public:
        GatewayService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);

        auto Run() -> Future<void> override;
        void Shutdown() override;
        void Join(boost::system::error_code& ec) override;

        auto GetName() const -> std::string_view override;

    public:
        auto AddLobby(int8_t serverId, std::string address) -> Future<void>;
        auto AddZone(int8_t serverId, std::string address, uint16_t port, int32_t zoneId) -> Future<void>;

        auto FindZone(int8_t serverId, int32_t zoneId) const -> Future<std::optional<std::pair<std::string, uint16_t>>>;

        auto GetLobbies() const -> Future<std::vector<std::pair<int8_t, std::string>>>;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        std::vector<std::pair<int8_t, std::string>> _lobbies;
        std::unordered_map<int8_t, std::unordered_map<int32_t, std::pair<std::string, uint16_t>>> _zones;
    };
}

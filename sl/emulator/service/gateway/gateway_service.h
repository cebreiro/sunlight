#pragma once
#include "sl/emulator/service/emulation_service_interface.h"

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

        auto GetLobbies() const -> Future<std::vector<std::pair<int8_t, std::string>>>;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        std::vector<std::pair<int8_t, std::string>> _lobbies;
    };
}

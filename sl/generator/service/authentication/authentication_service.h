#pragma once
#include "sl/generator/service/emulation_service_interface.h"
#include "sl/generator/service/authentication/authentication_token.h"

namespace sunlight
{
    class AuthenticationService final
        : public IEmulationService
        , public std::enable_shared_from_this<AuthenticationService>
    {
    public:
        AuthenticationService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);
        ~AuthenticationService() override;

        auto Run() -> Future<void> override;
        void Shutdown() override;
        void Join(boost::system::error_code& ec) override;

        auto GetName() const -> std::string_view override;

    public:
        auto Add(int64_t accountId, game_client_id_type id) -> Future<std::shared_ptr<AuthenticationToken>>;
        auto Remove(SharedPtrNotNull<AuthenticationToken> token) -> Future<bool>;
        auto Extract(int64_t accountId) -> Future<std::shared_ptr<AuthenticationToken>>;

        auto Find(AuthenticationToken::Key key) const -> Future<std::shared_ptr<AuthenticationToken>>;
        auto FindByPlayerName(uint32_t keyLowValue, const std::string& playerName) -> Future<std::shared_ptr<AuthenticationToken>>;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        std::mt19937 _mt;

        std::unordered_map<std::string, SharedPtrNotNull<AuthenticationToken>> _tokens;
        std::unordered_map<int64_t, SharedPtrNotNull<AuthenticationToken>> _tokenAccountIndex;
        std::unordered_multimap<uint32_t, SharedPtrNotNull<AuthenticationToken>> _zoneTokenIndex;
    };
}

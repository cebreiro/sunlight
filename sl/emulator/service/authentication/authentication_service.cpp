#include "authentication_service.h"

#include "sl/emulator/service/hash/sha256_hash_service.h"

namespace sunlight
{
    AuthenticationService::AuthenticationService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _mt(std::random_device{}())
    {
    }

    AuthenticationService::~AuthenticationService()
    {
    }

    auto AuthenticationService::Run() -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        co_return;
    }

    void AuthenticationService::Shutdown()
    {
    }

    void AuthenticationService::Join(boost::system::error_code& ec)
    {
        (void)ec;
    }

    auto AuthenticationService::GetName() const -> std::string_view
    {
        return "authentication_service";
    }

    auto AuthenticationService::Add(int64_t accountId, game_client_id_type clientId) -> Future<std::shared_ptr<AuthenticationToken>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (_tokenAccountIndex.contains(accountId))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to add token. already exists. account_id: {}, client_id: {}",
                    GetName(), accountId, clientId));

            co_return std::shared_ptr<AuthenticationToken>{};
        }

        std::uniform_int_distribution<int32_t> dist;

        while (true)
        {
            const std::string str = fmt::format("{}:{}:{}", accountId, clientId.Unwrap(), dist(_mt));

            AuthenticationToken::Key key(dist(_mt), dist(_mt));

            auto [iter, inserted] = _tokens.try_emplace(key.ToString(), std::shared_ptr<AuthenticationToken>{});
            if (inserted)
            {
                iter->second = std::make_shared<AuthenticationToken>(key, accountId, clientId);

                (void)_tokenAccountIndex.try_emplace(accountId, iter->second);

                co_return iter->second;
            }
        }
    }

    auto AuthenticationService::Remove(SharedPtrNotNull<AuthenticationToken> token) -> Future<void>
    {
        if (!token)
        {
            co_return;
        }

        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (const size_t count = _tokens.erase(token->GetKey().ToString()); count <= 0)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to remove token. token: {}",
                    GetName(), *token));
        }

        if (const size_t count = _tokenAccountIndex.erase(token->GetAccountId()); count <= 0)
        {
            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to remove token from account index. token: {}",
                    GetName(), *token));
        }

        co_return;
    }

    auto AuthenticationService::Extract(int64_t accountId) -> Future<std::shared_ptr<AuthenticationToken>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        std::shared_ptr<AuthenticationToken> result;

        const auto iter = _tokenAccountIndex.find(accountId);
        if (iter != _tokenAccountIndex.end())
        {
            std::swap(iter->second, result);

            _tokenAccountIndex.erase(iter);
            _tokens.erase(result->GetKey().ToString());
        }

        co_return result;
    }

    auto AuthenticationService::Find(AuthenticationToken::Key key) const -> Future<std::shared_ptr<AuthenticationToken>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        const auto iter = _tokens.find(key.ToString());

        co_return iter != _tokens.end() ? iter->second : std::shared_ptr<AuthenticationToken>{};
    }
}

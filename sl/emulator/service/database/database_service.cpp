#include "database_service.h"

#include "shared/database/database_error.h"
#include "shared/database/connection/connection_pool.h"
#include "sl/emulator/service/database/stored_procedure/stored_procedure.hpp"

namespace sunlight
{
    DatabaseService::DatabaseService(const ServiceLocator& serviceLocator, execution::IExecutor& executor,
        SharedPtrNotNull<db::ConnectionPool> connectionPool)
        : _serviceLocator(serviceLocator)
        , _executor(executor.SharedFromThis())
        , _connectionPool(std::move(connectionPool))
    {
    }

    auto DatabaseService::Run() -> Future<void>
    {
        co_return;
    }

    void DatabaseService::Shutdown()
    {
    }

    void DatabaseService::Join(boost::system::error_code& ec)
    {
        (void)ec;
    }

    auto DatabaseService::GetName() const -> std::string_view
    {
        return "database_service";
    }

    auto DatabaseService::CreateAccount(std::string account, std::string password, int8_t gmLevel) -> Future<std::optional<db::dto::Account>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::AccountAdd accountAdd(conn, std::move(account), std::move(password));

        if (const DatabaseError error = co_await accountAdd.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return accountAdd.GetResult();
    }

    auto DatabaseService::FindAccount(std::string account) -> Future<std::optional<db::dto::Account>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::AccountGet accountGet(conn, account);

        if (const DatabaseError error = co_await accountGet.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return accountGet.GetResult();
    }

    auto DatabaseService::GetCharacterNamesAll() -> Future<std::vector<std::string>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharactersNameGet charactersNameGet(conn);

        if (const DatabaseError error = co_await charactersNameGet.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return {};
        }

        co_return charactersNameGet.GetResult();
    }

    auto DatabaseService::GetLobbyCharacters(int64_t aid, int8_t sid) -> Future<std::optional<std::vector<db::dto::LobbyCharacter>>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::LobbyCharactersGet lobbyCharactersGet(conn, aid, sid);

        if (const DatabaseError error = co_await lobbyCharactersGet.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return std::ranges::to<std::vector>(lobbyCharactersGet.GetResult());
    }

    auto DatabaseService::CreateCharacter(req::CharacterCreate request) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();

        nlohmann::json j;
        to_json(j, request);

        db::sp::CharacterCreate characterCreate(conn, j.dump());

        if (const DatabaseError error = co_await characterCreate.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::DeleteCharacterSoft(int64_t cid) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterDeleteSoft characterDeleteSoft(conn, cid);

        if (const DatabaseError error = co_await characterDeleteSoft.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::GetCharacter(int64_t cid) -> Future<std::optional<db::dto::Character>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterGet characterGet(conn, cid);

        if (const DatabaseError error = co_await characterGet.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return characterGet.Release();
    }

    auto DatabaseService::SetCharacterExp(int64_t cid, int32_t exp) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSetExp characterSetExp(conn, cid, exp);

        if (const DatabaseError error = co_await characterSetExp.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetCharacterLevel(int64_t cid, int32_t level, int32_t statPoint) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSetLevel characterSetLevel(conn, cid, level, statPoint);

        if (const DatabaseError error = co_await characterSetLevel.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetJobExp(int64_t cid, int32_t job, int32_t exp) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSetJobExp characterSetJobExp(conn, cid, job, exp);

        if (const DatabaseError error = co_await characterSetJobExp.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetJobLevel(int64_t cid, int32_t job, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSetJobLevel characterSetJobLevel(conn, cid, job, level, skillPoint, skills);

        if (const DatabaseError error = co_await characterSetJobLevel.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;

    }

    auto DatabaseService::SetStat(int64_t cid, int32_t statPoint, int32_t str, int32_t dex, int32_t accr,
        int32_t health, int32_t intell, int32_t wis, int32_t will) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSetStat characterSetStat(conn, cid, statPoint, str, dex, accr, health, intell, wis, will);

        if (const DatabaseError error = co_await characterSetStat.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::StartTransaction(db::ItemTransaction transaction) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::ItemTransactionExecute itemTransactionExecute(conn, transaction);

        if (const DatabaseError error = co_await itemTransactionExecute.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    void DatabaseService::LogError(std::string_view function, const DatabaseError& error, const std::optional<std::string>& additionalLog)
    {
        if (additionalLog.has_value())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] {} query error. {}, error: {}",
                    GetName(), function, *additionalLog, error.What()));
        }
        else
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] {} query error. error: {}",
                    GetName(), function, error.What()));
        }
    }
}

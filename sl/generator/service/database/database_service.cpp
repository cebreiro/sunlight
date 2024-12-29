#include "database_service.h"

#include "shared/database/database_error.h"
#include "shared/database/connection/connection_pool.h"
#include "sl/generator/service/database/stored_procedure/stored_procedure.hpp"

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
        (void)gmLevel;

        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::AccountAdd procedure(conn, std::move(account), std::move(password));

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return procedure.GetResult();
    }

    auto DatabaseService::ChangeAccountPassword(std::string account, std::string password) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::AccountPasswordChange procedure(conn, std::move(account), std::move(password));

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::FindAccount(std::string account) -> Future<std::optional<db::dto::Account>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::AccountGet procedure(conn, account);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return procedure.GetResult();
    }

    auto DatabaseService::GetCharacterNamesAll() -> Future<std::vector<std::string>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharactersNameGet procedure(conn);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return {};
        }

        co_return procedure.GetResult();
    }

    auto DatabaseService::GetLobbyCharacters(int64_t aid, int8_t sid) -> Future<std::optional<std::vector<db::dto::LobbyCharacter>>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::LobbyCharactersGet procedure(conn, aid, sid);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return std::ranges::to<std::vector>(procedure.GetResult());
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

        db::sp::CharacterCreate procedure(conn, j.dump());

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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
        db::sp::CharacterDeleteSoft procedure(conn, cid);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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
        db::sp::CharacterGet procedure(conn, cid);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return procedure.Release();
    }

    auto DatabaseService::GetAccountStorage(int64_t aid) -> Future<std::optional<db::dto::AccountStorage>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::AccountStorageGet procedure(conn, aid);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::nullopt;
        }

        co_return std::move(procedure.GetResult());
    }

    auto DatabaseService::GetProfileIntroduction(int64_t cid) -> Future<std::pair<bool, std::optional<db::dto::ProfileIntroduction>>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterProfileIntroductionGet procedure(conn, cid);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return std::make_pair(false, std::optional<db::dto::ProfileIntroduction>());
        }

        co_return std::make_pair(true, std::move(procedure.GetResult()));
    }

    auto DatabaseService::AddNewJob(int64_t cid, int32_t job, int32_t jobType, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterJobAdd procedure(conn, cid, job, jobType, level, skillPoint, skills);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::AddQuest(int64_t cid, int32_t id, int32_t state, std::string flags, std::string data) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterQuestAdd procedure(conn, cid, id, state, std::move(flags), std::move(data));

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::AddSkill(int64_t cid, int32_t job, int32_t skillId, int32_t skillLevel) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSkillAdd procedure(conn, cid,job, skillId, skillLevel);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetCharacterExp(int64_t cid, int32_t exp) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterExpSet procedure(conn, cid, exp);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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
        db::sp::CharacterLevelSet procedure(conn, cid, level, statPoint);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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
        db::sp::CharacterJobExpSet procedure(conn, cid, job, exp);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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
        db::sp::CharacterJobLevelSet characterSetJobLevel(conn, cid, job, level, skillPoint, skills);

        if (const DatabaseError error = co_await characterSetJobLevel.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;

    }

    auto DatabaseService::SetSkillLevel(int64_t cid, int32_t job, int32_t skillPoint, int32_t skillId, int32_t skillLevel) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSkillLevelSet procedure(conn, cid, job, skillPoint, skillId, skillLevel);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetMixSkillExp(int64_t cid, int32_t skillId, int32_t skillLevel, int32_t exp) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterMixSkillExpSet procedure(conn, cid, skillId, skillLevel, exp);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetSkillPosition(int64_t cid, int32_t skillId, int8_t page, int8_t x, int8_t y) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterSkillPositionSet procedure(conn, cid, skillId, page, x, y);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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
        db::sp::CharacterStatSet procedure(conn, cid, statPoint, str, dex, accr, health, intell, wis, will);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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
        db::sp::ItemTransactionExecute procedure(conn, transaction);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
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

    auto DatabaseService::SetQuest(int64_t cid, int32_t id, int32_t state, std::string flags, std::string data) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterQuestSet procedure(conn, cid, id, state, std::move(flags), std::move(data));

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetHair(int64_t cid, int32_t hair) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterHairSet procedure(conn, cid, hair);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetHairColor(int64_t cid, int32_t hairColor) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterHairColorSet procedure(conn, cid, hairColor);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SetProfile(int64_t cid, int8_t refusePartyInvite, int8_t refuseChannelInvite,
        int8_t refuseGuildInvite, int8_t privateProfile, std::string age, std::string sex, std::string mail,
        std::string message) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterProfileSet procedure(conn, cid, refusePartyInvite, refuseChannelInvite, refuseGuildInvite, privateProfile,
            age, sex, mail, message);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }

    auto DatabaseService::SaveState(int64_t cid, int32_t zone, int32_t stage, float x, float y, float yaw, int8_t arms, int8_t running, int8_t dead, int32_t hp, int32_t sp)  -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_executor;
        assert(ExecutionContext::IsEqualTo(*_executor));

        db::ConnectionPool::Borrowed conn = co_await _connectionPool->Borrow();
        db::sp::CharacterStateSave procedure(conn, cid, zone, stage, x, y, yaw, arms, running, dead, hp, sp);

        if (const DatabaseError error = co_await procedure.ExecuteAsync(); error)
        {
            LogError(__FUNCTION__, error);

            co_return false;
        }

        co_return true;
    }
}

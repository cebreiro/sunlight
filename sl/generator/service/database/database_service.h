#pragma once
#include "sl/generator/service/emulation_service_interface.h"
#include "sl/generator/service/database/dto/account.h"
#include "sl/generator/service/database/dto/account_storage.h"
#include "sl/generator/service/database/dto/character.h"
#include "sl/generator/service/database/dto/lobby_character.h"
#include "sl/generator/service/database/dto/profile_introduction.h"
#include "sl/generator/service/database/request/character_create.h"
#include "sl/generator/service/database/transaction/transaction.hpp"

namespace sunlight
{
    class DatabaseError;
}

namespace sunlight::db
{
    class ConnectionPool;
}

namespace sunlight
{
    class DatabaseService final
        : public IEmulationService
        , public std::enable_shared_from_this<DatabaseService>
    {
    public:
        DatabaseService(const ServiceLocator& serviceLocator, execution::IExecutor& executor, SharedPtrNotNull<db::ConnectionPool> connectionPool);

        auto Run() -> Future<void> override;
        void Shutdown() override;
        void Join(boost::system::error_code& ec) override;

        auto GetName() const->std::string_view override;

    public:
        auto CreateAccount(std::string account, std::string password, int8_t gmLevel) -> Future<std::optional<db::dto::Account>>;
        auto ChangeAccountPassword(std::string account, std::string password) -> Future<bool>;
        auto FindAccount(std::string account) -> Future<std::optional<db::dto::Account>>;

        auto GetCharacterNamesAll() -> Future<std::vector<std::string>>;

        auto GetLobbyCharacters(int64_t aid, int8_t sid) -> Future<std::optional<std::vector<db::dto::LobbyCharacter>>>;
        auto CreateCharacter(req::CharacterCreate request) -> Future<bool>;
        auto DeleteCharacterSoft(int64_t cid) -> Future<bool>;

        auto GetCharacter(int64_t cid) -> Future<std::optional<db::dto::Character>>;
        auto GetAccountStorage(int64_t aid) -> Future<std::optional<db::dto::AccountStorage>>;
        auto GetProfileIntroduction(int64_t cid) -> Future<std::pair<bool, std::optional<db::dto::ProfileIntroduction>>>;

        auto AddNewJob(int64_t cid, int32_t job, int32_t jobType, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills) -> Future<bool>;
        auto AddQuest(int64_t cid, int32_t id, int32_t state, std::string flags, std::string data) -> Future<bool>;
        auto AddSkill(int64_t cid, int32_t job, int32_t skillId, int32_t skillLevel) -> Future<bool>;

        auto SetCharacterExp(int64_t cid, int32_t exp) -> Future<bool>;
        auto SetCharacterLevel(int64_t cid, int32_t level, int32_t statPoint) -> Future<bool>;
        auto SetJobExp(int64_t cid, int32_t job, int32_t exp) -> Future<bool>;
        auto SetJobLevel(int64_t cid, int32_t job, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills) -> Future<bool>;
        auto SetSkillLevel(int64_t cid, int32_t job, int32_t skillPoint, int32_t skillId, int32_t skillLevel) -> Future<bool>;
        auto SetMixSkillExp(int64_t cid, int32_t skillId, int32_t skillLevel, int32_t exp) -> Future<bool>;
        auto SetSkillPosition(int64_t cid, int32_t skillId, int8_t page, int8_t x, int8_t y) -> Future<bool>;

        auto SetStat(int64_t cid, int32_t statPoint, int32_t str, int32_t dex, int32_t accr, int32_t health, int32_t intell, int32_t wis, int32_t will) -> Future<bool>;
        auto SetQuest(int64_t cid, int32_t id, int32_t state, std::string flags, std::string data) -> Future<bool>;

        auto SetHair(int64_t cid, int32_t hair) -> Future<bool>;
        auto SetHairColor(int64_t cid, int32_t hairColor) -> Future<bool>;

        auto SetProfile(int64_t cid,
            int8_t refusePartyInvite, int8_t refuseChannelInvite, int8_t refuseGuildInvite, int8_t privateProfile,
            std::string age, std::string sex, std::string mail, std::string message) -> Future<bool>;

        auto SaveState(int64_t cid, int32_t zone, int32_t stage, float x, float y, float yaw, int8_t arms, int8_t running, int8_t dead, int32_t hp, int32_t sp) -> Future<bool>;

        auto StartTransaction(db::ItemTransaction transaction) -> Future<bool>;

    private:
        void LogError(std::string_view function, const DatabaseError& error, const std::optional<std::string>& additionalLog = std::nullopt);

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;
    };
}

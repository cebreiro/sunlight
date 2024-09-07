#pragma once
#include "sl/emulator/service/emulation_service_interface.h"
#include "sl/emulator/service/database/dto/account.h"
#include "sl/emulator/service/database/dto/character.h"
#include "sl/emulator/service/database/dto/lobby_character.h"
#include "sl/emulator/service/database/request/character_create.h"

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
        auto FindAccount(std::string account) -> Future<std::optional<db::dto::Account>>;

        auto GetCharacterNamesAll() -> Future<std::vector<std::string>>;

        auto GetLobbyCharacters(int64_t aid, int8_t sid) -> Future<std::optional<std::vector<db::dto::LobbyCharacter>>>;
        auto CreateCharacter(req::CharacterCreate request) -> Future<bool>;
        auto DeleteCharacterSoft(int64_t cid) -> Future<bool>;

        auto GetCharacter(int64_t cid) -> Future<std::optional<db::dto::Character>>;

    private:
        void LogError(std::string_view function, const DatabaseError& error, const std::optional<std::string>& additionalLog = std::nullopt);

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;
    };
}

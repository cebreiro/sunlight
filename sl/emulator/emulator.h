#pragma once
#include "shared/app/app_intance.h"
#include "sl/emulator/app_config.h"

namespace sunlight::db
{
    class ConnectionPool;
}

namespace sunlight
{
    class GameDataProvideService;
    class GameClientStorage;
    class SafeHashService;
    class Sha256HashService;
    class SnowflakeService;
    class UniqueNameService;
}

namespace sunlight
{
    class IEmulationService;

    class AuthenticationService;
    class DatabaseService;
    class GatewayService;
}

namespace sunlight
{
    class LoginServer;
    class LobbyServer;
    class ZoneServer;
}

namespace sunlight
{
    class SlEmulator final : public AppInstance
    {
    public:
        SlEmulator();
        ~SlEmulator();

        auto GetName() const -> std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

        template <typename T>
        void RegisterService(const SharedPtrNotNull<T>& service);

    private:
        static auto InitializeConfig() -> sl::emulator::AppConfig;

        void InitializeLogger();
        void InitializeExecutor();
        void InitializeDatabaseConnection();
        void InitializeGameData();
        void InitializeService();
        void InitializeServer();

    private:
        sl::emulator::AppConfig _config;

        SharedPtrNotNull<execution::AsioExecutor> _ioExecutor;
        SharedPtrNotNull<execution::GameExecutor> _gameExecutor;
        SharedPtrNotNull<LogService> _logService;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;

        SharedPtrNotNull<GameDataProvideService> _gameDataProvideService;
        SharedPtrNotNull<GameClientStorage> _gameClientStorage;
        SharedPtrNotNull<SafeHashService> _safeHashService;
        SharedPtrNotNull<Sha256HashService> _sha256HashService;
        SharedPtrNotNull<SnowflakeService> _snowflakeService;
        SharedPtrNotNull<UniqueNameService> _uniqueNameService;

        SharedPtrNotNull<AuthenticationService> _authenticationService;
        SharedPtrNotNull<DatabaseService> _databaseService;
        SharedPtrNotNull<GatewayService> _gatewayService;
        std::vector<SharedPtrNotNull<IEmulationService>> _services;

        SharedPtrNotNull<LoginServer> _loginServer;
        SharedPtrNotNull<LobbyServer> _lobbyServer;
        std::unordered_map<int8_t, std::unordered_map<int32_t, SharedPtrNotNull<ZoneServer>>> _zoneServers;
    };
}

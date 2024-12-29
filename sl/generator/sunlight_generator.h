#pragma once
#include "shared/app/app_intance.h"
#include "sl/generator/app_config.h"

namespace sunlight::db
{
    class ConnectionPool;
}

namespace sunlight
{
    class ConfigProvideService;
    class GameDataProvideService;
    class GameClientStorage;
    class SafeHashService;
    class Sha256HashService;
    class SnowflakeService;
    class UniqueNameService;
    class GameCheatLogService;
    class GeneratorControlService;
    class LogMemCacheService;
}

namespace sunlight
{
    class IEmulationService;

    class AuthenticationService;
    class DatabaseService;
    class GatewayService;
    class CommunityService;
    class WorldService;
}

namespace sunlight
{
    class LoginServer;
    class LobbyServer;
    class ZoneServer;
}

namespace sunlight
{
    class SunlightGenerator final : public AppInstance
    {
    public:
        SunlightGenerator();
        ~SunlightGenerator();

        auto GetName() const -> std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

        template <typename T>
        void RegisterService(const SharedPtrNotNull<T>& service);

    private:
        static auto FindBasePath() -> std::filesystem::path;
        auto CreateConfig() const -> AppConfig;

        void InitializeConfig();
        void InitializeLogger();
        void InitializeExecutor();
        void InitializeDatabaseConnection();
        void InitializeGameCheatLogger();
        void InitializeGameData();
        void InitializeService();
        void InitializeServer();

    private:
        std::filesystem::path _basePath;
        AppConfig _appConfig;

        SharedPtrNotNull<execution::AsioExecutor> _ioExecutor;
        SharedPtrNotNull<execution::GameExecutor> _gameExecutor;
        SharedPtrNotNull<LogService> _logService;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;

        SharedPtrNotNull<ConfigProvideService> _configProvideService;
        SharedPtrNotNull<GameDataProvideService> _gameDataProvideService;
        SharedPtrNotNull<GameClientStorage> _gameClientStorage;
        SharedPtrNotNull<SafeHashService> _safeHashService;
        SharedPtrNotNull<Sha256HashService> _sha256HashService;
        SharedPtrNotNull<SnowflakeService> _snowflakeService;
        SharedPtrNotNull<UniqueNameService> _uniqueNameService;
        SharedPtrNotNull<GameCheatLogService> _gameCheatLogService;
        SharedPtrNotNull<GeneratorControlService> _generatorControlService;
        SharedPtrNotNull<LogMemCacheService> _logMemCacheService;

        SharedPtrNotNull<AuthenticationService> _authenticationService;
        SharedPtrNotNull<DatabaseService> _databaseService;
        SharedPtrNotNull<GatewayService> _gatewayService;
        SharedPtrNotNull<CommunityService> _communityService;
        SharedPtrNotNull<WorldService> _worldService;
        std::vector<SharedPtrNotNull<IEmulationService>> _services;

        SharedPtrNotNull<LoginServer> _loginServer;
        SharedPtrNotNull<LobbyServer> _lobbyServer;
    };
}

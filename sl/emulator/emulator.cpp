#include "emulator.h"

#include <boost/filesystem/operations.hpp>

#include "shared/database/connection/connection_pool.h"
#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/execution/executor/impl/game_executor.h"
#include "shared/log/spdlog/spdlog_logger_builder.h"
#include "sl/emulator/server/login_server.h"
#include "sl/emulator/server/lobby_server.h"
#include "sl/emulator/server/server_constant.h"
#include "sl/emulator/server/zone_server.h"
#include "sl/emulator/server/client/game_client_storage.h"
#include "sl/emulator/service/authentication/authentication_service.h"
#include "sl/emulator/service/community/community_service.h"
#include "sl/emulator/service/config/config_provide_service.h"
#include "sl/emulator/service/database/database_service.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gateway/gateway_service.h"
#include "sl/emulator/service/hash/safe_hash_service.h"
#include "sl/emulator/service/hash/sha256_hash_service.h"
#include "sl/emulator/service/snowflake/snowflake_service.h"
#include "sl/emulator/service/unique_name/unique_name_service.h"

namespace
{
    constexpr const char* configFileName = "app_config.json";
}

namespace sunlight
{
    SlEmulator::SlEmulator()
        : _basePath(FindBasePath())
        , _appConfig(CreateConfig())
        , _ioExecutor(std::make_shared<execution::AsioExecutor>(_appConfig.ioThreadCount))
        , _gameExecutor(std::make_shared<execution::GameExecutor>(_appConfig.gameThreadCount))
        , _logService(std::make_shared<LogService>())
        , _connectionPool(std::make_shared<db::ConnectionPool>(_ioExecutor))
        , _configProvideService(std::make_shared<ConfigProvideService>())
        , _gameDataProvideService(std::make_shared<GameDataProvideService>(GetServiceLocator()))
        , _gameClientStorage(std::make_shared<GameClientStorage>())
        , _safeHashService(std::make_shared<SafeHashService>(GetServiceLocator(), *_ioExecutor))
        , _sha256HashService(std::make_shared<Sha256HashService>(GetServiceLocator(), *_ioExecutor))
        , _snowflakeService(std::make_shared<SnowflakeService>(GetServiceLocator(), *_ioExecutor))
        , _uniqueNameService(std::make_shared<UniqueNameService>(GetServiceLocator(), *_ioExecutor))
        , _authenticationService(std::make_shared<AuthenticationService>(GetServiceLocator(), *_ioExecutor))
        , _databaseService(std::make_shared<DatabaseService>(GetServiceLocator(), *_ioExecutor, _connectionPool))
        , _gatewayService(std::make_shared<GatewayService>(GetServiceLocator(), *_ioExecutor))
        , _communityService(std::make_shared<CommunityService>(GetServiceLocator(), *_ioExecutor))
        , _loginServer(std::make_shared<LoginServer>(*_ioExecutor))
        , _lobbyServer(std::make_shared<LobbyServer>(*_ioExecutor))
    {
        RegisterService(_configProvideService);
        RegisterService(_gameDataProvideService);
        RegisterService(_gameClientStorage);
        RegisterService(_safeHashService);
        RegisterService(_sha256HashService);
        RegisterService(_snowflakeService);
        RegisterService(_uniqueNameService);

        RegisterService(_authenticationService);
        RegisterService(_databaseService);
        RegisterService(_gatewayService);
        RegisterService(_communityService);
    }

    SlEmulator::~SlEmulator()
    {
    }

    auto SlEmulator::GetName() const -> std::string_view
    {
        return "sl_emulator";
    }

    void SlEmulator::OnStartUp(std::span<char*> args)
    {
        (void)args;

        InitializeConfig();
        InitializeLogger();
        InitializeExecutor();
        InitializeDatabaseConnection();
        InitializeGameData();
        InitializeService();
        InitializeServer();
    }

    void SlEmulator::OnShutdown()
    {
    }

    template <typename T>
    void SlEmulator::RegisterService(const SharedPtrNotNull<T>& service)
    {
        static_assert(std::derived_from<T, IService>);

        GetServiceLocator().Add<T>(service);

        if constexpr (std::derived_from<T, IEmulationService>)
        {
            _services.emplace_back(service);
        }
    }

    auto SlEmulator::FindBasePath() -> std::filesystem::path
    {
        std::unordered_set<std::filesystem::path> visited;

        std::queue<std::filesystem::path> queue;
        queue.emplace(std::filesystem::current_path());

        const std::filesystem::path root = queue.front().root_path();

        while (!queue.empty())
        {
            std::filesystem::path current = queue.front();
            queue.pop();

            if (current == root)
            {
                break;
            }

            if (!visited.emplace(current).second)
            {
                continue;
            }

            if (is_directory(current))
            {
                queue.emplace(current.parent_path());

                for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(current))
                {
                    if (entry.is_directory())
                    {
                        continue;
                    }

                    if (const std::filesystem::path filePath = entry.path();
                        filePath.filename().compare(configFileName) == 0)
                    {
                        return filePath.parent_path();
                    }
                }
            }
            else
            {
                if (current.filename().compare(configFileName) == 0)
                {
                    return current.parent_path();
                }
            }
        }

        throw std::runtime_error("fail to find base path");
    }

    auto SlEmulator::CreateConfig() const -> AppConfig
    {
        AppConfig result;

        std::filesystem::path configPath = _basePath / configFileName;
        assert(exists(configPath));

        std::ifstream ifs(configPath);
        if (!ifs.is_open())
        {
            throw std::runtime_error("fail to open config file");
        }

        nlohmann::json json = nlohmann::json::parse(ifs);
        json.get_to<AppConfig>(result);

        return result;
    }

    void SlEmulator::InitializeConfig()
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize config", GetName()));

        const std::filesystem::path gameConfigPath = _basePath / _appConfig.pathConfig.gameConfigFilePath;
        if (!exists(gameConfigPath))
        {
            throw std::runtime_error(fmt::format("fail to find game config file path. path: {}", gameConfigPath.string()));
        }

        _configProvideService->Initialize(_appConfig, gameConfigPath);

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize config --> Done", GetName()));
    }

    void SlEmulator::InitializeLogger()
    {
        const std::filesystem::path& logFilePath = _basePath / _appConfig.pathConfig.logFilePath;

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Debug).SetAsync(false);
        builder.ConfigureDailyFile().SetLogLevel(LogLevel::Info).SetPath(logFilePath);

        _logService->Add(-1, builder.CreateLogger());

        GetServiceLocator().Add<ILogService>(_logService);

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize logger --> Done. log_file_path: {}",
                GetName(), logFilePath.string()));
    }

    void SlEmulator::InitializeExecutor()
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize executor", GetName()));

        ExecutionContext::PushExecutor(_ioExecutor.get());
        _ioExecutor->Run();

        _gameExecutor->Run();

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize executor --> Done", GetName()));
    }

    void SlEmulator::InitializeDatabaseConnection()
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize database connection", GetName()));

        const DatabaseConfig& databaseConfig = _appConfig.databaseConfig;

        const auto endPoint = boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address(databaseConfig.address), databaseConfig.port);
        const boost::mysql::handshake_params params(databaseConfig.user, databaseConfig.password, databaseConfig.schema);

        db::ConnectionPoolOption option(endPoint, params, databaseConfig.connectionCount);

        _connectionPool->Initialize(GetServiceLocator());
        _connectionPool->Start(option);

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize database connection --> Done", GetName()));
    }

    void SlEmulator::InitializeGameData()
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize game data", GetName()));

        const std::filesystem::path assetPath = _basePath / _appConfig.pathConfig.assetPath;
        if (!exists(assetPath))
        {
            throw std::runtime_error(fmt::format("fail to find asset path. path: {}", assetPath.string()));
        }

        _gameDataProvideService->Initialize(assetPath);

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize game data --> Done", GetName()));
    }

    void SlEmulator::InitializeService()
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize service", GetName()));

        for (const WorldConfig& worldConfig : _appConfig.worldConfig)
        {
            _gatewayService->AddLobby(worldConfig.id, _appConfig.publicAddress);

            for (const ZoneConfig& zoneConfig : worldConfig.zoneConfig)
            {
                _gatewayService->AddZone(worldConfig.id, _appConfig.publicAddress, zoneConfig.port, zoneConfig.zoneId);
            }
        }

        for (const SharedPtrNotNull<IEmulationService>& service : _services)
        {
            service->Run();
        }

        _uniqueNameService->InitializeName(*_databaseService).Get();

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize service --> Done", GetName()));
    }

    void SlEmulator::InitializeServer()
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize server", GetName()));

        for (const WorldConfig& worldConfig : _appConfig.worldConfig)
        {
            auto& zoneServers = _zoneServers[worldConfig.id];

            for (const ZoneConfig& zoneConfig : worldConfig.zoneConfig)
            {
                auto zoneServer = std::make_shared<ZoneServer>(*_ioExecutor, *_gameExecutor, worldConfig.id, zoneConfig.zoneId);

                zoneServer->Initialize(GetServiceLocator());
                zoneServer->StartUp(zoneConfig.port);

                zoneServers[zoneConfig.zoneId] = std::move(zoneServer);
            }
        }

        _lobbyServer->Initialize(GetServiceLocator());
        _lobbyServer->StartUp(ServerConstant::LOBBY_PORT);

        _loginServer->Initialize(GetServiceLocator());
        _loginServer->StartUp(ServerConstant::LOGIN_PORT);

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize server --> Done", GetName()));
    }
}

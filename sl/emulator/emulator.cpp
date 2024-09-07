#include "emulator.h"

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
#include "sl/emulator/service/database/database_service.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gateway/gateway_service.h"
#include "sl/emulator/service/hash/safe_hash_service.h"
#include "sl/emulator/service/hash/sha256_hash_service.h"
#include "sl/emulator/service/snowflake/snowflake_service.h"
#include "sl/emulator/service/unique_name/unique_name_service.h"

namespace sunlight
{
    SlEmulator::SlEmulator()
        : _config(InitializeConfig())
        , _ioExecutor(std::make_shared<execution::AsioExecutor>(_config.ioThreadCount))
        , _gameExecutor(std::make_shared<execution::GameExecutor>(_config.gameThreadCount))
        , _logService(std::make_shared<LogService>())
        , _connectionPool(std::make_shared<db::ConnectionPool>(_ioExecutor))
        , _gameDataProvideService(std::make_shared<GameDataProvideService>(GetServiceLocator()))
        , _gameClientStorage(std::make_shared<GameClientStorage>())
        , _safeHashService(std::make_shared<SafeHashService>(GetServiceLocator(), *_ioExecutor))
        , _sha256HashService(std::make_shared<Sha256HashService>(GetServiceLocator(), *_ioExecutor))
        , _snowflakeService(std::make_shared<SnowflakeService>(GetServiceLocator(), *_ioExecutor))
        , _uniqueNameService(std::make_shared<UniqueNameService>(GetServiceLocator(), *_ioExecutor))
        , _authenticationService(std::make_shared<AuthenticationService>(GetServiceLocator(), *_ioExecutor))
        , _databaseService(std::make_shared<DatabaseService>(GetServiceLocator(), *_ioExecutor, _connectionPool))
        , _gatewayService(std::make_shared<GatewayService>(GetServiceLocator(), *_ioExecutor))
        , _loginServer(std::make_shared<LoginServer>(*_ioExecutor))
        , _lobbyServer(std::make_shared<LobbyServer>(*_ioExecutor))
    {
        RegisterService(_gameDataProvideService);
        RegisterService(_gameClientStorage);
        RegisterService(_safeHashService);
        RegisterService(_sha256HashService);
        RegisterService(_snowflakeService);
        RegisterService(_uniqueNameService);

        RegisterService(_authenticationService);
        RegisterService(_databaseService);
        RegisterService(_gatewayService);
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

    auto SlEmulator::InitializeConfig() -> sl::emulator::AppConfig
    {
        sl::emulator::AppConfig result;

        auto path = std::filesystem::current_path();
        for (; exists(path) && path != path.parent_path(); path = path.parent_path())
        {
            std::filesystem::path configPath = path / "sl/emulator/app_config.json";
            if (!exists(configPath))
            {
                continue;
            }

            std::ifstream ifs(configPath);
            if (!ifs.is_open())
            {
                throw std::runtime_error("fail to open config file");
            }

            nlohmann::json json = nlohmann::json::parse(ifs);
            json.get_to<sl::emulator::AppConfig>(result);

            return result;
        }

        throw std::runtime_error("fail to initialize config");
    }

    void SlEmulator::InitializeLogger()
    {
        const std::filesystem::path& logFilePath = std::filesystem::current_path() / _config.logFilePath;

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

        const sl::emulator::DatabaseConfig& databaseConfig = _config.databaseConfig;

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

        const auto assetPath = [&]() -> std::optional<std::filesystem::path>
            {
                auto path = std::filesystem::current_path();
                for (; exists(path) && path != path.parent_path(); path = path.parent_path())
                {
                    std::filesystem::path result = path / "asset";
                    if (exists(result))
                    {
                        return result;
                    }
                }

                return std::nullopt;
            }();

        if (!assetPath.has_value())
        {
            throw std::runtime_error("fail to find asset path");
        }

        _gameDataProvideService->Initialize(*assetPath);

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize game data --> Done", GetName()));
    }

    void SlEmulator::InitializeService()
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] initialize service", GetName()));

        for (const sl::emulator::WorldConfig& worldConfig : _config.worldConfig)
        {
            _gatewayService->AddLobby(worldConfig.id, _config.publicAddress);

            for (const sl::emulator::ZoneConfig& zoneConfig : worldConfig.zoneConfig)
            {
                _gatewayService->AddZone(worldConfig.id, _config.publicAddress, zoneConfig.port, zoneConfig.zoneId);
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

        for (const sl::emulator::WorldConfig& worldConfig : _config.worldConfig)
        {
            auto& zoneServers = _zoneServers[worldConfig.id];

            for (const sl::emulator::ZoneConfig& zoneConfig : worldConfig.zoneConfig)
            {
                auto zoneServer = std::make_shared<ZoneServer>(*_ioExecutor, *_gameExecutor, zoneConfig.zoneId);

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

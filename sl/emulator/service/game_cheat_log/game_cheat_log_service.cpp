#include "game_cheat_log_service.h"

#include "shared/log/spdlog/spdlog_logger_builder.h"

namespace sunlight
{
    GameCheatLogService::GameCheatLogService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
    {
    }

    void GameCheatLogService::Initialize(const std::filesystem::path& cheatLogBasePath)
    {
        _basePath = cheatLogBasePath;
    }

    auto GameCheatLogService::Run() -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        while (!_shutdown.load())
        {
            co_await Delay(std::chrono::seconds(10));
            assert(ExecutionContext::IsEqualTo(*_strand));

            const auto now = std::chrono::system_clock::now();

            for (auto iter = _loggers.begin(); iter != _loggers.end(); )
            {
                const std::chrono::system_clock::time_point logTimePoint = iter->second.second;

                if (now - logTimePoint >= std::chrono::seconds(10))
                {
                    iter = _loggers.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }
    }

    void GameCheatLogService::Shutdown()
    {
        _shutdown.store(true);
    }

    void GameCheatLogService::Join(boost::system::error_code& ec)
    {
        (void)ec;
    }

    auto GameCheatLogService::GetName() const -> std::string_view
    {
        return "game_cheat_log_service";
    }

    void GameCheatLogService::Log(GameCheatLogType type, std::string playerName, std::string message)
    {
        Post(*_strand,
            [self = shared_from_this(), type, name = std::move(playerName), message = std::move(message)]()
            {
                self->LogImpl(type, name, message);
            });
    }

    void GameCheatLogService::LogImpl(GameCheatLogType type, const std::string& playerName, const std::string& message)
    {
        auto iter = _loggers.find(playerName);
        if (iter == _loggers.end())
        {
            iter = _loggers.try_emplace(playerName, CreateLogger(playerName), std::chrono::system_clock::time_point::min()).first;
            assert(iter != _loggers.end());
        }

        auto& [logger, logTimePoint] = iter->second;

        logger->Log(LogLevel::Info, fmt::format("[{}] {}", ToString(type), message), std::source_location::current());
        logTimePoint = std::chrono::system_clock::now();
    }

    auto GameCheatLogService::CreateLogger(const std::string& playerName) const -> SharedPtrNotNull<ILogger>
    {
        SpdLogLoggerBuilder builder;
        builder.ConfigureDailyFile()
            .SetLogLevel(LogLevel::Debug)
            .SetAsync(false)
            .SetPattern("[%Y-%m-%d %H:%M:%S.%e] %v")
            .SetPath(_basePath / fmt::format("{}/log.txt", playerName));

        return builder.CreateLogger();
    }
}

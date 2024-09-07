#pragma once
#include <memory>
#include <optional>
#include <mutex>

#include "shared/log/logger_interface.h"
#include "shared/log/spdlog/spdlog_logger_build_config.h"
#include "shared/type/not_null_pointer.h"

namespace sunlight
{
    class SpdLogLogger;

    class SpdLogLoggerBuilder
    {
    public:
        SpdLogLoggerBuilder();

        void SetConfig(const SpdLogConsoleLoggerConfig& config);
        void SetConfig(const SpdLogDailyFileLoggerConfig& config);

        auto ConfigureConsole() -> SpdLogConsoleLoggerConfig&;
        auto ConfigureDailyFile() -> SpdLogDailyFileLoggerConfig&;

        auto CreateLogger() const -> SharedPtrNotNull<ILogger>;

    private:
        static void InitializeSpdLog();

    private:
        std::optional<SpdLogConsoleLoggerConfig> _consoleConfig;
        std::optional<SpdLogDailyFileLoggerConfig> _dailyFileConfig;

        static std::once_flag _initFlag;
    };
}

#pragma once
#include <memory>
#include "shared/log/logger_interface.h"

namespace spdlog
{
    class logger;
    class async_logger;

    namespace sinks
    {
        class sink;
    }
}

namespace sunlight
{
    class SpdLogConsoleLoggerConfig;
    class SpdLogDailyFileLoggerConfig;

    class SpdLogLogger final : public ILogger
    {
    public:
        SpdLogLogger() = delete;

        SpdLogLogger(std::shared_ptr<spdlog::logger> logger, std::shared_ptr<spdlog::async_logger> asyncLogger);
        ~SpdLogLogger() override;

        void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) override;
        void Flush() override;

    private:
        std::shared_ptr<spdlog::logger> _syncLogger;
        std::shared_ptr<spdlog::async_logger> _asyncLogger;
    };
}

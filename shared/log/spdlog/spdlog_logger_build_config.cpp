#include "spdlog_logger_build_config.h"

namespace sunlight
{
    SpdLogConsoleLoggerConfig& SpdLogConsoleLoggerConfig::SetColor(LogLevel logLevel, uint16_t color)
    {
        _colors[logLevel] = color;

        return *this;
    }

    auto SpdLogConsoleLoggerConfig::GetColors() const noexcept -> const std::map<LogLevel, uint16_t>&
    {
        return _colors;
    }

    SpdLogDailyFileLoggerConfig::SpdLogDailyFileLoggerConfig()
    {
        SetAsync(true);
        SetPattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] [%!] %v");
    }

    SpdLogDailyFileLoggerConfig& SpdLogDailyFileLoggerConfig::SetPath(std::filesystem::path path)
    {
        _path = std::move(path);

        return *this;
    }

    auto SpdLogDailyFileLoggerConfig::GetPath() const -> const std::filesystem::path&
    {
        return _path;
    }
}

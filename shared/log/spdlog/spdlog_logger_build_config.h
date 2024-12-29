#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <filesystem>
#include "shared/log/log_level.h"

namespace sunlight
{
    namespace detail
    {
        template <typename T>
        class SpdlogLoggerBaseBuildConfig
        {
            friend class SpdLogServiceBuilder;

        public:
            auto SetLogLevel(LogLevel logLevel) -> T&;
            auto SetAsync(bool async) -> T&;
            auto SetPattern(std::string pattern) -> T&;

            auto GetLogLevel() const noexcept -> LogLevel;
            bool IsAsync() const noexcept;
            auto GetPattern() const noexcept -> const std::string&;

        private:
            LogLevel _logLevel = LogLevel::Debug;
            bool _async = false;
            std::string _pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v";
        };

        template <typename T>
        auto SpdlogLoggerBaseBuildConfig<T>::SetLogLevel(LogLevel logLevel) -> T&
        {
            _logLevel = logLevel;

            return static_cast<T&>(*this);
        }

        template <typename T>
        auto SpdlogLoggerBaseBuildConfig<T>::SetAsync(bool async) -> T&
        {
            _async = async;

            return static_cast<T&>(*this);
        }

        template <typename T>
        auto SpdlogLoggerBaseBuildConfig<T>::SetPattern(std::string pattern) -> T&
        {
            _pattern = std::move(pattern);

            return static_cast<T&>(*this);
        }

        template <typename T>
        auto SpdlogLoggerBaseBuildConfig<T>::GetLogLevel() const noexcept -> LogLevel
        {
            return _logLevel;
        }

        template <typename T>
        bool SpdlogLoggerBaseBuildConfig<T>::IsAsync() const noexcept
        {
            return _async;
        }

        template <typename T>
        auto SpdlogLoggerBaseBuildConfig<T>::GetPattern() const noexcept -> const std::string&
        {
            return _pattern;
        }
    }

    class SpdLogConsoleLoggerConfig : public detail::SpdlogLoggerBaseBuildConfig<SpdLogConsoleLoggerConfig>
    {
        friend class SpdLogLoggerBuilder;

    public:
        SpdLogConsoleLoggerConfig& SetColor(LogLevel logLevel, uint16_t color);

        auto GetColors() const noexcept -> const std::map<LogLevel, uint16_t>&;

    private:
        std::map<LogLevel, uint16_t> _colors;
    };

    class SpdLogDailyFileLoggerConfig : public detail::SpdlogLoggerBaseBuildConfig<SpdLogDailyFileLoggerConfig>
    {
        friend class SpdLogLoggerBuilder;

    public:
        SpdLogDailyFileLoggerConfig();

        SpdLogDailyFileLoggerConfig& SetPath(std::filesystem::path path);

        auto GetPath() const -> const std::filesystem::path&;

    private:
        std::filesystem::path _path = std::filesystem::current_path() / "log";
    };
}

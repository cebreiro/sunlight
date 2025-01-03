#pragma once
#include <concepts>
#include <algorithm>

namespace sunlight
{
    enum class LogLevel
    {
        Debug,
        Info,
        Warn,
        Error,
        Critical,
    };

    template <std::integral T>
    auto ToLogLevel(T value) -> LogLevel
    {
        constexpr int64_t min = static_cast<int64_t>(LogLevel::Debug);
        constexpr int64_t max = static_cast<int64_t>(LogLevel::Critical);

        return static_cast<LogLevel>(std::clamp<int64_t>(value, min, max));
    }

    inline bool IsValidLogLevel(int32_t value)
    {
        switch (static_cast<LogLevel>(value))
        {
        case LogLevel::Debug:
        case LogLevel::Info:
        case LogLevel::Warn:
        case LogLevel::Error:
        case LogLevel::Critical:
            return true;
        }

        return false;
    }
}

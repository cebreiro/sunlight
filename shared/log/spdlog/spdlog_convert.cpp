#include "spdlog_convert.h"

namespace sunlight::detail
{
    auto ToSpdLog(LogLevel level) -> spdlog::level::level_enum
    {
        switch (level)
        {
        case LogLevel::Debug: return spdlog::level::level_enum::debug;
        case LogLevel::Info: return spdlog::level::level_enum::info;
        case LogLevel::Warn: return spdlog::level::level_enum::warn;
        case LogLevel::Error: return spdlog::level::level_enum::err;
        case LogLevel::Critical: return spdlog::level::level_enum::critical;
        default:
            assert(false);
            return spdlog::level::level_enum::debug;
        }
    }

    auto ToSpdLog(const std::source_location& location) -> spdlog::source_loc
    {
        return spdlog::source_loc{
            location.file_name(),
            static_cast<int>(location.line()),
            location.function_name()
        };
    }
}

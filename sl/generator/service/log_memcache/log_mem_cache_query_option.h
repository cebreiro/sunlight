#pragma once
#include "sl/generator/service/log_memcache/log_mem_cache_type.h"

namespace sunlight
{
    struct LogMemCacheQueryOption
    {
        using date_time_type = std::chrono::system_clock::time_point;

        LogLevel logLevel = {};
        std::optional<date_time_type> dateLowerBound = std::nullopt;
        std::optional<date_time_type> dateUpperBound = std::nullopt;
    };
}

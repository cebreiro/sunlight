#pragma once
#include "sl/generator/service/log_memcache/log_mem_cache_type.h"

namespace sunlight
{
    struct LogMemCacheQueryResult
    {
        LogLevel logLevel = {};
        log_mem_cache_date_time_type dateTime = {};
        std::string log;
    };
}

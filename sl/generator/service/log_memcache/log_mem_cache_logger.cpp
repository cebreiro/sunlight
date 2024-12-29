#include "log_mem_cache_logger.h"

#include "sl/generator/service/log_memcache/log_mem_cache_service.h"

namespace sunlight
{
    LogMemCacheLogger::LogMemCacheLogger(std::weak_ptr<LogMemCacheService> logMemCacheService)
        : _weak(std::move(logMemCacheService))
    {
    }

    void LogMemCacheLogger::Log(LogLevel logLevel, const std::string& message, const std::source_location& location)
    {
        (void)location;

        auto logMemCacheService = _weak.lock();
        if (!logMemCacheService)
        {
            return;
        }

        LogMemCacheService::AddLog(std::move(logMemCacheService), logLevel, message);
    }

    void LogMemCacheLogger::Flush()
    {
    }
}

#pragma once
#include "sl/generator/service/log_memcache/log_mem_cache_query_result.h"

namespace sunlight
{
    struct LogMemCacheQueryOption;
    class LogRingBuffer;
    class LogMemCacheLogger;
}

namespace sunlight
{
    class LogMemCacheService final
        : public IService
        , public std::enable_shared_from_this<LogMemCacheService>
    {
    public:
        LogMemCacheService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);

        void Initialize(LogService& logService);

        auto GetName() const -> std::string_view override;

    public:
        void AddLog(LogLevel logLevel, std::string log);
        auto GetLog(std::vector<LogMemCacheQueryResult>& result, const LogMemCacheQueryOption& option) -> Future<void>;

        static void AddLog(SharedPtrNotNull<LogMemCacheService> logMemCacheService, LogLevel logLevel, std::string log);

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        SharedPtrNotNull<LogMemCacheLogger> _memCacheLogger;
        SharedPtrNotNull<LogRingBuffer> _ringBuffer;
    };
}

#include "log_mem_cache_service.h"

#include "sl/generator/service/log_memcache/log_mem_cache_logger.h"
#include "sl/generator/service/log_memcache/log_ring_buffer.h"

namespace sunlight
{
    LogMemCacheService::LogMemCacheService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
    {
    }

    void LogMemCacheService::Initialize(LogService& logService)
    {
        _memCacheLogger = std::make_shared<LogMemCacheLogger>(weak_from_this());

        // TODO: replace constant to configuration value;
        _ringBuffer = std::make_shared<LogRingBuffer>(1024);

        // TODO: fix this;
        constexpr int64_t key = -2;

        logService.Add(key, _memCacheLogger);
    }

    auto LogMemCacheService::GetName() const -> std::string_view
    {
        return "log_mem_cache_service";
    }

    void LogMemCacheService::AddLog(LogLevel logLevel, std::string log)
    {
        AddLog(shared_from_this(), logLevel, std::move(log));
    }

    auto LogMemCacheService::GetLog(std::vector<LogMemCacheQueryResult>& result, const LogMemCacheQueryOption& option) -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;

        _ringBuffer->Get(result, option);

        co_return;
    }

    void LogMemCacheService::AddLog(SharedPtrNotNull<LogMemCacheService> logMemCacheService, LogLevel logLevel, std::string log)
    {
        Strand& strand = *logMemCacheService->_strand;

        Post(strand, [self = std::move(logMemCacheService), logLevel, log = std::move(log)]() mutable
            {
                self->_ringBuffer->Add(logLevel, std::move(log));
            });
    }
}

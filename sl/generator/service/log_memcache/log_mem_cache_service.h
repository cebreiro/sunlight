#pragma once

namespace sunlight
{
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

        static void AddLog(SharedPtrNotNull<LogMemCacheService> logMemCacheService, LogLevel logLevel, std::string log);

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        SharedPtrNotNull<LogMemCacheLogger> _memCacheLogger;
        SharedPtrNotNull<LogRingBuffer> _ringBuffer;
    };
}

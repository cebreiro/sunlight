#pragma once

namespace sunlight
{
    class LogMemCacheService;
}

namespace sunlight
{
    class LogMemCacheLogger final : public ILogger
    {
    public:
        explicit LogMemCacheLogger(std::weak_ptr<LogMemCacheService> logMemCacheService);

        void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) override;
        void Flush() override;

    private:
        std::weak_ptr<LogMemCacheService> _weak;
    };
}

#pragma once
#include <tbb/concurrent_unordered_map.h>
#include "shared/log/log_service_interface.h"
#include "shared/type/not_null_pointer.h"

namespace sunlight
{
    class LogService final : public ILogService
    {
    public:
        void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) override;
        void Flush() override;

        bool Add(int64_t key, SharedPtrNotNull<ILogger> logger) override;
        auto Find(int64_t key) -> ILogger* override;

        auto GetName() const -> std::string_view override;

    private:
        tbb::concurrent_unordered_map<int64_t, SharedPtrNotNull<ILogger>> _loggers;
    };
}

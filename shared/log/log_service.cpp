#include "log_service.h"

#include <cassert>
#include "shared/type/not_null_pointer.h"

namespace sunlight
{
    void LogService::Log(LogLevel logLevel, const std::string& message, const std::source_location& location)
    {
        for (ILogger& logService : _loggers | std::views::values | notnull::reference)
        {
            logService.Log(logLevel, message, location);
        }
    }

    void LogService::Flush()
    {
        for (ILogger& logService : _loggers | std::views::values | notnull::reference)
        {
            logService.Flush();
        }
    }

    bool LogService::Add(int64_t key, SharedPtrNotNull<ILogger> logger)
    {
        assert(logger);
        return _loggers.emplace(key, std::move(logger)).second;
    }

    auto LogService::Find(int64_t key) -> ILogger*
    {
        auto iter = _loggers.find(key);
        return iter != _loggers.end() ? iter->second.get() : nullptr;
    }

    auto LogService::GetName() const -> std::string_view
    {
        return "log_service";
    }
}

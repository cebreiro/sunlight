#include "spdlog_logger.h"

#include "spdlog/async.h"
#include "shared/log/spdlog/spdlog_convert.h"

namespace sunlight
{
    SpdLogLogger::SpdLogLogger(std::shared_ptr<spdlog::logger> logger, std::shared_ptr<spdlog::async_logger> asyncLogger)
        : _syncLogger(std::move(logger))
        , _asyncLogger(std::move(asyncLogger))
    {
        if (_syncLogger)
        {
            spdlog::register_logger(_syncLogger);
        }

        if (_asyncLogger)
        {
            spdlog::register_logger(_asyncLogger);
        }
    }

    SpdLogLogger::~SpdLogLogger()
    {
        if (_syncLogger)
        {
            spdlog::drop(_syncLogger->name());
        }

        if (_asyncLogger)
        {
            spdlog::drop(_asyncLogger->name());
        }
    }

    void SpdLogLogger::Log(LogLevel logLevel, const std::string& message, const std::source_location& location)
    {
        if (_syncLogger)
        {
            _syncLogger->log(detail::ToSpdLog(location), detail::ToSpdLog(logLevel), message);
        }

        if (_asyncLogger)
        {
            _asyncLogger->log(detail::ToSpdLog(location), detail::ToSpdLog(logLevel), message);
        }
    }

    void SpdLogLogger::Flush()
    {
        if (_syncLogger)
        {
            _syncLogger->flush();
        }

        if (_asyncLogger)
        {
            _asyncLogger->flush();
        }
    }
}

#pragma once
#include <string>
#include <source_location>
#include <fmt/format.h>
#include "shared/app/app.h"
#include "shared/log/log_service.h"


#define SUNLIGHT_APP_LOG(logLevel, message) \
if (ILogService* logService = App::Find<ILogService>(); logService != nullptr) \
    logService->Log(logLevel, message, std::source_location::current())


#define SUNLIGHT_APP_LOG_DEBUG(message)  SUNLIGHT_APP_LOG(sunlight::LogLevel::Debug, message)
#define SUNLIGHT_APP_LOG_INFO(message)  SUNLIGHT_APP_LOG(sunlight::LogLevel::Info, message)
#define SUNLIGHT_APP_LOG_WAN(message)  SUNLIGHT_APP_LOG(sunlight::LogLevel::Warn, message)
#define SUNLIGHT_APP_LOG_ERROR(message)  SUNLIGHT_APP_LOG(sunlight::LogLevel::Error, message)
#define SUNLIGHT_APP_LOG_CRITICAL(message)  SUNLIGHT_APP_LOG(sunlight::LogLevel::Critical, message)

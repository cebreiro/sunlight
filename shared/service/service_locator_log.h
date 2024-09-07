#pragma once
#include <string>
#include <source_location>
#include <fmt/format.h>
#include "shared/app/app.h"
#include "shared/log/log_service.h"


#define SUNLIGHT_LOG_IMPL(locator, logLevel, message) \
if (ILogService* logService = locator.Find<ILogService>(); logService != nullptr) \
    logService->Log(logLevel, message, std::source_location::current())


#define SUNLIGHT_LOG_DEBUG(locator, message)  SUNLIGHT_LOG_IMPL(locator, sunlight::LogLevel::Debug, message)
#define SUNLIGHT_LOG_INFO(locator, message)  SUNLIGHT_LOG_IMPL(locator, sunlight::LogLevel::Info, message)
#define SUNLIGHT_LOG_WARN(locator, message)  SUNLIGHT_LOG_IMPL(locator, sunlight::LogLevel::Warn, message)
#define SUNLIGHT_LOG_ERROR(locator, message)  SUNLIGHT_LOG_IMPL(locator, sunlight::LogLevel::Error, message)
#define SUNLIGHT_LOG_CRITICAL(locator, message)  SUNLIGHT_LOG_IMPL(locator, sunlight::LogLevel::Critical, message)

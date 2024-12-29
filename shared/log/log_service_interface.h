#pragma once
#include <cstdint>
#include "shared/service/service_interface.h"
#include "shared/log/logger_interface.h"
#include "shared/type/not_null_pointer.h"

namespace sunlight
{
    class ILogService : public ILogger, public IService
    {
    public:
        virtual ~ILogService() = default;

        virtual bool Add(int64_t key, SharedPtrNotNull<ILogger> logger) = 0;
        virtual auto Find(int64_t key) -> ILogger* = 0;
    };
}

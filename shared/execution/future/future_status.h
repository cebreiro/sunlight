#pragma once
#include <cstdint>

namespace sunlight
{
    enum class FutureStatus : int32_t
    {
        Pending,
        Complete,
        Canceled,
    };
}

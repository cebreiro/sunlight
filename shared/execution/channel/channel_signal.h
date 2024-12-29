#pragma once
#include <cstdint>

namespace sunlight::channel
{
    enum class ChannelSignal : int32_t
    {
        None = 0,
        NotifyOne,
        NotifyAll,
    };
}

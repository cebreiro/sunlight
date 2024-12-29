#pragma once
#include <concepts>
#include "shared/execution/channel/impl/mpmc_channel.h"

namespace sunlight
{
    template <std::move_constructible T>
    using Channel = channel::MpmcChannel<T>;
}

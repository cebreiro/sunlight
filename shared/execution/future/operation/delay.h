#pragma once
#include <chrono>
#include "shared/execution/future/future.h"

namespace sunlight
{
    auto Delay(std::chrono::milliseconds milliseconds) -> Future<void>;
}

#pragma once
#include <cstdint>
#include <optional>

namespace sunlight
{
    auto QueryCPULoadPercentage() -> std::optional<int32_t>;
    auto QueryFreePhysicalMemoryGB() -> std::optional<double>;
}

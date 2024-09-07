#include "query_sys_info.h"

#include <cassert>
#include <charconv>
#include "shared/process/execute_command.h"

namespace sunlight
{
    auto QueryCPULoadPercentage() -> std::optional<int32_t>
    {
        std::vector<std::string> results = ExecuteCommand("wmic cpu get LoadPercentage");
        if (std::ssize(results) < 2)
        {
            return std::nullopt;
        }

        const std::string& str = results.at(1);

        int32_t converted = 0;

        [[maybe_unused]]
        const std::errc ec = std::from_chars(str.data(), str.data() + str.size(), converted).ec;
        assert(ec == std::errc());

        return converted;
    }

    auto QueryFreePhysicalMemoryGB() -> std::optional<double>
    {
        std::vector<std::string> results = ExecuteCommand("wmic OS get FreePhysicalMemory");
        if (std::ssize(results) < 2)
        {
            return std::nullopt;
        }

        const std::string& str = results.at(1);

        double conv = 0;

        [[maybe_unused]]
        const std::errc ec = std::from_chars(str.data(), str.data() + str.size(), conv).ec;
        assert(ec == std::errc());

        return conv / (1024 * 1024);
    }
}

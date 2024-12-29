#pragma once
#include <string>
#include <vector>

namespace sunlight
{
    auto ExecuteCommand(const std::string& cmd) -> std::vector<std::string>;
}

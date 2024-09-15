#include "server_command_param_reader.h"

namespace sunlight
{
    ServerCommandParamReader::ServerCommandParamReader(const std::span<std::string>& params)
        : _params(params)
    {
    }

    auto ServerCommandParamReader::GetParamSize() const -> int64_t
    {
        return std::ssize(_params);
    }
}

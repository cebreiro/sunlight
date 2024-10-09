#include "server_command_param_reader.h"

namespace sunlight
{
    ServerCommandParamReader::ServerCommandParamReader(const std::span<std::string>& params)
        : _params(params)
    {
    }

    bool ServerCommandParamReader::Get(int64_t index, std::string& result) const
    {
        assert(index >= 0);

        if (index >= std::ssize(_params))
        {
            return false;
        }

        result = _params[index];

        return true;
    }

    auto ServerCommandParamReader::GetParamSize() const -> int64_t
    {
        return std::ssize(_params);
    }
}

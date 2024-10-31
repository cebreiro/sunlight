#include "nes_script_call.h"

namespace sunlight
{
    NesScriptCall::NesScriptCall(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        callerId = reader.Read<int32_t>();
        callerType = reader.Read<int32_t>();

        const int32_t count = reader.Read<int32_t>();
        scriptCalls.reserve(count);

        for (int32_t i = 0; i < count; ++i)
        {
            const int32_t type = reader.Read<int32_t>();
            const int32_t scriptId = reader.Read<int32_t>();

            scriptCalls.emplace_back(type, scriptId);
        }
    }
}

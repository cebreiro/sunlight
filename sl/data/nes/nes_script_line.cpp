#include "nes_script_line.h"

namespace sunlight
{
    NesScriptLine::NesScriptLine(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        type = reader.Read<int32_t>();

        const int32_t tokenCount = reader.Read<int32_t>();
        tokens.reserve(tokenCount);

        for (int32_t i = 0; i < tokenCount; ++i)
        {
            tokens.emplace_back(reader);
        }
    }
}

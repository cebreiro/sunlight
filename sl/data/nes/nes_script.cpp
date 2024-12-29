#include "nes_script.h"

namespace sunlight
{
    NesScript::NesScript(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        id = reader.Read<int32_t>();
        description = reader.ReadString(reader.Read<int32_t>());

        const int32_t lineCount = reader.Read<int32_t>();
        lines.reserve(lineCount);

        for (int32_t i = 0; i < lineCount; ++i)
        {
            lines.emplace_back(reader);
        }
    }
}

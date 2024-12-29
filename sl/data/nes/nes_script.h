#pragma once
#include "sl/data/nes/nes_script_line.h"

namespace sunlight
{
    struct NesScript
    {
        NesScript() = default;
        explicit NesScript(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t id = 0;
        std::string description;

        std::vector<NesScriptLine> lines;
    };
}

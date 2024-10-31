#pragma once
#include "sl/data/nes/nes_script_token.h"

namespace sunlight
{
    struct NesScriptLine
    {
        NesScriptLine() = default;
        explicit NesScriptLine(StreamReader<std::vector<char>::const_iterator>& reader);

        // 2    -> comment
        // 7    -> exit
        // 8    -> if
        // 9    -> {
        // 10   -> }
        // 11   -> else
        // 20   -> A = B
        // 21   -> A = B operator C
        // 400  -> func_400
        // 1200 -> func_1200
        int32_t type = 0;

        std::vector<NesScriptToken> tokens;
    };
}

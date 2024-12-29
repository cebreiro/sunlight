#pragma once

namespace sunlight
{
    struct NesScriptCall
    {
        NesScriptCall() = default;
        explicit NesScriptCall(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t callerId = 0;
        int32_t callerType = 0;

        // type, script_id pair
        // type 102 -> spawner, npc talk, ...
        // type 201 -> stage/zone enter/exit
        std::vector<std::pair<int32_t, int32_t>> scriptCalls;
    };
}

#pragma once
#include "sl/data/nes/nes_script.h"
#include "sl/data/nes/nes_script_call.h"

namespace sunlight
{
    struct NesFile
    {
        int32_t unk1 = 0;
        int32_t unk2 = 0;

        std::vector<NesScript> scripts;
        std::unordered_map<int32_t, PtrNotNull<const NesScript>> scriptIdIndex;

        int32_t unk3 = 0;
        std::vector<NesScriptCall> scriptCalls;
        std::unordered_map<int32_t, PtrNotNull<const NesScriptCall>> callerIdIndex;

        static auto CreateFrom(const std::filesystem::path& filePath) -> NesFile;
    };
}

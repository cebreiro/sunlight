#pragma once
#include "sl/data/abf/ability_routine.h"

namespace sunlight
{
    ENUM_CLASS(AbilityFileVersion, int32_t,
        (V1_0)
        (V1_1)
        (V1_2)
    )

    struct AbilityFile
    {
        int32_t assetId = 0;

        std::string name;
        AbilityFileVersion version = AbilityFileVersion::V1_0;
        bool version1_2 = false;

        float unk1 = 0.f;
        std::array<int32_t, 4> unk2 = {};

        std::vector<AbilityRoutine> routines;

        static constexpr int64_t HEADER_SIZE = 296;

        static auto CreateFrom(const std::filesystem::path& filePath) -> AbilityFile;
    };
}

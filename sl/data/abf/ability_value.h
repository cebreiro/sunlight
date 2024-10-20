#pragma once

namespace sunlight
{
    struct AbilityValue
    {
        int32_t type = 0;
        int32_t unk1 = 0;
        int32_t begin = 0;
        int32_t end = 0;
        int32_t unk4 = 0;
        std::string assetName;
        int32_t unk5 = 0;

        std::array<char, 128> buffer;
    };
}

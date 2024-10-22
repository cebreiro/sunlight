#pragma once

namespace sunlight
{
    struct AbilityValue
    {
        // 96
        int32_t type = 0;
        int32_t unk1 = 0;
        int32_t begin = 0;
        int32_t end = 0;
        int32_t unk4 = 0;
        std::string assetName;
        int32_t unk5 = 0;

        // 128
        int32_t otherUnk1 = 0;
        int32_t otherUnk2 = 0;
        float otherUnk3 = 0;
        int32_t damageCount = 0;
        std::array<char, 112> buffer;
    };
}

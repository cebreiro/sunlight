#pragma once

namespace sunlight
{
    struct MapTexture
    {
        MapTexture() = default;
        explicit MapTexture(StreamReader<std::vector<char>::const_iterator>& reader);

        std::array<char, 64> name = {};
        int32_t style = 0;
        Eigen::Vector3f unk1;
        Eigen::Vector3f unk2;
    };
}

#pragma once

namespace sunlight
{
    struct MapEventObjectV3
    {
        MapEventObjectV3() = default;
        explicit MapEventObjectV3(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t id = 0;
        int32_t type = 0;
        Eigen::Matrix4f transform;
        Eigen::Vector3f vector;
        int32_t style = 0;
    };

    struct MapEventObjectV5
    {
        MapEventObjectV5() = default;
        explicit MapEventObjectV5(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t id = 0;
        int32_t type = 0;
        Eigen::Matrix4f transform;
        Eigen::Vector3f vector;
        int32_t style = 0;

        int32_t reserved1 = 0;
        int32_t reserved2 = 0;
        int32_t unk1 = 0;
        int32_t unk2 = 0;
        std::array<char, 192> desc = {};
    };
}

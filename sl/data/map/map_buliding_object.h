#pragma once

namespace sunlight
{
    struct MapBuildingObject
    {
        MapBuildingObject() = default;
        explicit MapBuildingObject(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t id = 0;
        int32_t type = 0;
        int32_t pnx = 0;

        Eigen::Matrix4f transform;
        int32_t unk1 = 0;
        int32_t unk2 = 0;
        int32_t unk3 = 0;
        int32_t unk4 = 0;
        int32_t unk5 = 0;
        int32_t unk6 = 0;
    };
}

#pragma once
#include "sl/data/map/map_event_object.h"
#include "sl/data/map/map_prop.h"
#include "sl/data/map/map_texture.h"

namespace sunlight
{
    struct MapStageRoom
    {
        MapStageRoom() = default;
        explicit MapStageRoom(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t unk1 = 0; // float?
        Eigen::Matrix4f unk2;
        std::array<char, 6> desc = {};
        int32_t version = 0;
        int32_t width = 0;
        int32_t height = 0;

        int32_t blockCount = 0;
        int32_t pnx = 0;

        std::vector<char> movableData; // width * height
        std::vector<MapTexture> textures;
        std::vector<char> unk3;
        std::vector<MapProp> props;

        std::array<char, 5> eventDesc = {};
        std::vector<MapEventObjectV5> events;
    };
}

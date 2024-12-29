#pragma once

namespace sunlight
{
    struct MapStageTerrain;
    struct MapStageRoom;
}

namespace sunlight
{
    struct MapStage
    {
        MapStage() = default;
        explicit MapStage(StreamReader<std::vector<char>::const_iterator>& reader);

        ~MapStage();

        int32_t flag = 0;
        int32_t size = 0;
        int32_t id = 0;
        int32_t type = 0;
        std::array<char, 20> desc = {};

        std::unique_ptr<MapStageTerrain> terrain;
        std::unique_ptr<MapStageRoom> room;
    };
}

#pragma once
#include "sl/data/map/map_event_object.h"
#include "sl/data/map/map_prop.h"
#include "sl/data/map/map_buliding_object.h"

namespace sunlight
{
    struct MapStageTerrain
    {
        MapStageTerrain() = default;
        explicit MapStageTerrain(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t width = 0;
        int32_t height = 0;

        std::array<char, 72> moveDataDesc = {};
        std::vector<char> movableData;

        std::array<char, 10> version = {};
        std::array<int32_t, 7> data = {};
        std::vector<char> unkData;

        std::array<char, 5> evtDesc = {};
        std::vector<MapEventObjectV3> eventsV3;
        std::vector<MapEventObjectV5> eventsV5;

        std::array<char, 4> propDesc = {};
        std::vector<MapTerrainProp> props;

        std::array<char, 8> buildingDesc = {};
        std::vector<MapBuildingObject> buildings;
    };
}

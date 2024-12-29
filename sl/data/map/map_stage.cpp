#include "map_stage.h"

#include "sl/data/map/map_stage_room.h"
#include "sl/data/map/map_stage_terrain.h"

namespace sunlight
{
    MapStage::MapStage(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        flag = reader.Read<int32_t>();
        size = reader.Read<int32_t>();
        id = reader.Read<int32_t>();;
        type = reader.Read<int32_t>();

        reader.ReadBuffer(desc.data(), std::ssize(desc));

        switch (type)
        {
        case 1910: // GameEntityType::StageTerrain
        {
            terrain = std::make_unique<MapStageTerrain>(reader);
        }
        break;
        case 1920: // GameEntityType::StageRoom
        {
            room = std::make_unique<MapStageRoom>(reader);
        }
        break;
        default:;
        }

        assert(terrain || room);
    }

    MapStage::~MapStage()
    {
    }
}

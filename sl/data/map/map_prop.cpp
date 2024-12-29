#include "map_prop.h"

#include "sl/data/map/map_stream_utility.h"

namespace sunlight
{
    MapProp::MapProp(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        id = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        pnx = reader.Read<int32_t>();
        transform = ReadMatrix4x4(reader);
        unk1 = reader.Read<int32_t>();
        unk2 = reader.Read<int32_t>();
        unk3 = reader.Read<int32_t>();
        unk4 = reader.Read<int32_t>();
        unk5 = reader.Read<int32_t>();
    }

    MapTerrainProp::MapTerrainProp(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        id = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        pnx = reader.Read<int32_t>();
        transform = ReadMatrix4x4(reader);
        unk1 = reader.Read<int32_t>();
        unk2 = reader.Read<int32_t>();
        unk3 = reader.Read<int32_t>();
        unk4 = reader.Read<int32_t>();
        unk5 = reader.Read<int32_t>();
        unk6 = reader.Read<int32_t>();
    }
}

#include "map_event_object.h"

#include "sl/data/map/map_stream_utility.h"

namespace sunlight
{
    MapEventObjectV3::MapEventObjectV3(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        id = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        transform = ReadMatrix4x4(reader);
        vector = ReadVector3(reader);
        style = reader.Read<int32_t>();
    }

    MapEventObjectV5::MapEventObjectV5(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        id = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        transform = ReadMatrix4x4(reader);
        vector = ReadVector3(reader);
        style = reader.Read<int32_t>();

        reserved1 = reader.Read<int32_t>();
        reserved2 = reader.Read<int32_t>();
        unk1 = reader.Read<int32_t>();
        unk2 = reader.Read<int32_t>();

        reader.ReadBuffer(desc.data(), std::ssize(desc));
    }
}

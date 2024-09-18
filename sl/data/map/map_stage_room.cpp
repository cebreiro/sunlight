#include "map_stage_room.h"

#include "sl/data/map/map_stream_utility.h"

namespace sunlight
{
    MapStageRoom::MapStageRoom(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        unk1 = reader.Read<int32_t>();
        unk2 = ReadMatrix4x4(reader);
        reader.ReadBuffer(desc.data(), std::ssize(desc));
        version = reader.Read<int32_t>();
        width = reader.Read<int32_t>();
        height = reader.Read<int32_t>();
        blockCount = reader.Read<int32_t>();

        const int32_t propCount = reader.Read<int32_t>();
        const int32_t textureCount = reader.Read<int32_t>();

        pnx = reader.Read<int32_t>();

        {
            const int32_t movableDataSize = width * height;

            movableData.resize(movableDataSize);
            reader.ReadBuffer(movableData.data(), std::ssize(movableData));
        }
        
        {
            textures.reserve(textureCount);

            for (int32_t i = 0; i < textureCount; ++i)
            {
                textures.emplace_back(reader);
            }
        }

        {
            const bool v2 = ::strcmp(desc.data(), "RoomV2") == 0;
            const int32_t unkSize = width * height * (v2 ? 13 : 1);

            unk3.resize(unkSize);
            reader.ReadBuffer(unk3.data(), std::ssize(unk3));
        }

        {
            props.reserve(propCount);

            for (int32_t i = 0; i < propCount; ++i)
            {
                props.emplace_back(reader);
            }
        }

        {
            reader.ReadBuffer(eventDesc.data(), std::ssize(eventDesc));

            const int32_t count = reader.Read<int32_t>();
            events.reserve(count);

            for (int32_t i = 0; i < count; ++i)
            {
                events.emplace_back(reader);
            }
        }
    }
}

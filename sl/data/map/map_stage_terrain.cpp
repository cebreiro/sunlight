#include "map_stage_terrain.h"

namespace sunlight
{
    MapStageTerrain::MapStageTerrain(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        width = reader.Read<int32_t>();
        height = reader.Read<int32_t>();

        {
            reader.ReadBuffer(moveDataDesc.data(), std::ssize(moveDataDesc));

            const int32_t size = reader.Read<int32_t>();

            movableData.resize(size);
            reader.ReadBuffer(movableData.data(), std::ssize(movableData));
        }

        {
            reader.ReadBuffer(version.data(), std::ssize(version));
            assert(::strcmp(version.data(), "TerrainV5") == 0 || ::strcmp(version.data(), "TerrainV3") == 0);

            const int32_t size = reader.Read<int32_t>();
            reader.ReadBuffer(reinterpret_cast<char*>(data.data()), sizeof(data));

            unkData.resize(size - 14 - 28);
            reader.ReadBuffer(unkData.data(), std::ssize(unkData));
        }

        {
            reader.ReadBuffer(evtDesc.data(), std::ssize(evtDesc));

            const int32_t count = reader.Read<int32_t>();
            
            const bool v3 = std::string(version.begin(), version.end()).find("V3") != std::string::npos;

            if (v3)
            {
                eventsV3.reserve(count);

                for (int32_t i = 0; i < count; ++i)
                {
                    eventsV3.emplace_back(reader);
                }

                
            }
            else
            {
                eventsV5.reserve(count);

                for (int32_t i = 0; i < count; ++i)
                {
                    eventsV5.emplace_back(reader);
                }
            }
        }

        {
            reader.ReadBuffer(propDesc.data(), std::ssize(propDesc));

            const int32_t count = reader.Read<int32_t>();

            props.reserve(count);

            for (int32_t i = 0; i < count; ++i)
            {
                props.emplace_back(reader);
            }
        }

        {
            reader.ReadBuffer(buildingDesc.data(), std::ssize(buildingDesc));

            const int32_t count = reader.Read<int32_t>();

            buildings.reserve(count);

            for (int32_t i = 0; i < count; ++i)
            {
                buildings.emplace_back(reader);
            }

        }
    }
}

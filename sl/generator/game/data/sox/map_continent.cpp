#include "map_continent.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MapContinent::MapContinent(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        continentId = reader.Read<int32_t>();
        zoneId = reader.Read<int32_t>();
        zoneX1 = reader.Read<int32_t>();
        zoneY1 = reader.Read<int32_t>();
        zoneX2 = reader.Read<int32_t>();
        zoneY2 = reader.Read<int32_t>();
        zoneNameX1 = reader.Read<int32_t>();
        zoneNameY1 = reader.Read<int32_t>();
        link01X1 = reader.Read<int32_t>();
        link01Y1 = reader.Read<int32_t>();
        link02X1 = reader.Read<int32_t>();
        link02Y1 = reader.Read<int32_t>();
        link03X1 = reader.Read<int32_t>();
        link03Y1 = reader.Read<int32_t>();
        link04X1 = reader.Read<int32_t>();
        link04Y1 = reader.Read<int32_t>();
        link05X1 = reader.Read<int32_t>();
        link05Y1 = reader.Read<int32_t>();
        link06X1 = reader.Read<int32_t>();
        link06Y1 = reader.Read<int32_t>();
        dungeon01String = reader.Read<int32_t>();
        dungeon01X1 = reader.Read<int32_t>();
        dungeon01Y1 = reader.Read<int32_t>();
        dungeon02String = reader.Read<int32_t>();
        dungeon02X1 = reader.Read<int32_t>();
        dungeon02Y1 = reader.Read<int32_t>();
        dungeon03String = reader.Read<int32_t>();
        dungeon03X1 = reader.Read<int32_t>();
        dungeon03Y1 = reader.Read<int32_t>();
        dungeon04String = reader.Read<int32_t>();
        dungeon04X1 = reader.Read<int32_t>();
        dungeon04Y1 = reader.Read<int32_t>();
        dungeon05String = reader.Read<int32_t>();
        dungeon05X1 = reader.Read<int32_t>();
        dungeon05Y1 = reader.Read<int32_t>();
    }

    void MapContinentTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MAP_CONTINENT.sox");
        StreamReader<std::vector<char>::const_iterator> reader(fileData.gameData.begin(), fileData.gameData.end());

        _vector.reserve(fileData.rowCount);
        for (int64_t i = 0; i < fileData.rowCount; ++i)
        {
            _vector.emplace_back(reader);
        }

        for (auto& data : _vector)
        {
            _umap[data.index] = &data;
        }
    }

    auto MapContinentTable::Find(int32_t index) const -> const MapContinent*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MapContinentTable::Get() const -> const std::vector<MapContinent>&
    {
        return _vector;
    }
}

#include "zone.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Zone::Zone(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        zoneFilename = reader.ReadString(reader.Read<uint16_t>());
        zoneType = reader.Read<int32_t>();
        zoneDesc = reader.ReadString(reader.Read<uint16_t>());
        zoneBgm = reader.Read<int32_t>();
        zoneWeatherDbid = reader.Read<int32_t>();
        zoneAvatarLight = reader.Read<int32_t>();
        zoneAmbient = reader.Read<int32_t>();
        zoneEntryLv = reader.Read<int32_t>();
        zoneUseability = reader.Read<int32_t>();
        zoneUsereturn = reader.Read<int32_t>();
    }

    void ZoneTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ZONE.sox");
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

    auto ZoneTable::Find(int32_t index) const -> const Zone*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ZoneTable::Get() const -> const std::vector<Zone>&
    {
        return _vector;
    }
}

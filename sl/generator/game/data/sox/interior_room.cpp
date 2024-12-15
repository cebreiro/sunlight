#include "interior_room.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    InteriorRoom::InteriorRoom(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        roomId = reader.Read<int32_t>();
        roomType = reader.Read<int32_t>();
        roomLv = reader.Read<int32_t>();
        roomCost = reader.Read<int32_t>();
        roomDesc = reader.Read<int32_t>();
        roomPreview = reader.Read<int32_t>();
    }

    void InteriorRoomTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "INTERIOR_ROOM.sox");
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

    auto InteriorRoomTable::Find(int32_t index) const -> const InteriorRoom*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto InteriorRoomTable::Get() const -> const std::vector<InteriorRoom>&
    {
        return _vector;
    }
}

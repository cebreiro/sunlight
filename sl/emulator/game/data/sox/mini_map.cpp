#include "mini_map.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MiniMap::MiniMap(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        toolTip = reader.ReadString(reader.Read<uint16_t>());
        icon1 = reader.Read<int32_t>();
        icon2 = reader.Read<int32_t>();
    }

    void MiniMapTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINI_MAP.sox");
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

    auto MiniMapTable::Find(int32_t index) const -> const MiniMap*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MiniMapTable::Get() const -> const std::vector<MiniMap>&
    {
        return _vector;
    }
}

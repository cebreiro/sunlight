#include "hair_color.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    HairColor::HairColor(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        pnxValue = reader.Read<int32_t>();
        r = reader.Read<int32_t>();
        g = reader.Read<int32_t>();
        b = reader.Read<int32_t>();
    }

    void HairColorTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "HAIR_COLOR.sox");
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

    auto HairColorTable::Find(int32_t index) const -> const HairColor*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto HairColorTable::Get() const -> const std::vector<HairColor>&
    {
        return _vector;
    }
}

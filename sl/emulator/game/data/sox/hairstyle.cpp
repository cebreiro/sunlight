#include "hairstyle.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Hairstyle::Hairstyle(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        id = reader.Read<int32_t>();
        sex = reader.Read<int32_t>();
        price = reader.Read<int32_t>();
    }

    void HairstyleTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "HAIRSTYLE.sox");
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

    auto HairstyleTable::Find(int32_t index) const -> const Hairstyle*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto HairstyleTable::Get() const -> const std::vector<Hairstyle>&
    {
        return _vector;
    }
}

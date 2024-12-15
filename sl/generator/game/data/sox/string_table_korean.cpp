#include "string_table_korean.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    StringTableKorean::StringTableKorean(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        string = reader.ReadString(reader.Read<uint16_t>());
        desc = reader.ReadString(reader.Read<uint16_t>());
    }

    void StringTableKoreanTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "StringTableKorean.sox");
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

    auto StringTableKoreanTable::Find(int32_t index) const -> const StringTableKorean*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto StringTableKoreanTable::Get() const -> const std::vector<StringTableKorean>&
    {
        return _vector;
    }
}

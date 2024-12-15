#include "itemmix_skillexp_table.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemmixSkillexpTable::ItemmixSkillexpTable(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        difficultyA = reader.Read<int32_t>();
        difficultyB = reader.Read<int32_t>();
        difficultyC = reader.Read<int32_t>();
    }

    void ItemmixSkillexpTableTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEMMIX_SKILLEXP_TABLE.sox");
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

    auto ItemmixSkillexpTableTable::Find(int32_t index) const -> const ItemmixSkillexpTable*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemmixSkillexpTableTable::Get() const -> const std::vector<ItemmixSkillexpTable>&
    {
        return _vector;
    }
}

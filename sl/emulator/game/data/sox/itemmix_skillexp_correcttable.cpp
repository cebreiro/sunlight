#include "itemmix_skillexp_correcttable.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemmixSkillexpCorrecttable::ItemmixSkillexpCorrecttable(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        difficultyA = reader.Read<int32_t>();
        difficultyB = reader.Read<int32_t>();
        difficultyC = reader.Read<int32_t>();
        difficultyD = reader.Read<int32_t>();
        difficultyE = reader.Read<int32_t>();
        difficultyF = reader.Read<int32_t>();
    }

    void ItemmixSkillexpCorrecttableTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEMMIX_SKILLEXP_CORRECTTABLE.sox");
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

    auto ItemmixSkillexpCorrecttableTable::Find(int32_t index) const -> const ItemmixSkillexpCorrecttable*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemmixSkillexpCorrecttableTable::Get() const -> const std::vector<ItemmixSkillexpCorrecttable>&
    {
        return _vector;
    }
}

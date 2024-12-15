#include "itemmix_judge_table.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemmixJudgeTable::ItemmixJudgeTable(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        fail = reader.Read<int32_t>();
        gradeLow = reader.Read<int32_t>();
        gradeModdle = reader.Read<int32_t>();
        gradeHigh = reader.Read<int32_t>();
        gradSUper = reader.Read<int32_t>();
    }

    void ItemmixJudgeTableTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEMMIX_JUDGE_TABLE.sox");
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

    auto ItemmixJudgeTableTable::Find(int32_t index) const -> const ItemmixJudgeTable*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemmixJudgeTableTable::Get() const -> const std::vector<ItemmixJudgeTable>&
    {
        return _vector;
    }
}

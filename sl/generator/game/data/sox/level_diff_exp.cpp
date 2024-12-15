#include "level_diff_exp.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    LevelDiffExp::LevelDiffExp(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        partyLevelDiffFactor01 = reader.Read<int32_t>();
        partyLevelDiffFactor02 = reader.Read<int32_t>();
        partyLevelDiffFactor03 = reader.Read<int32_t>();
        partyLevelDiffFactor04 = reader.Read<int32_t>();
        partyLevelDiffFactor05 = reader.Read<int32_t>();
        partyLevelDiffFactor06 = reader.Read<int32_t>();
        partyLevelDiffFactor07 = reader.Read<int32_t>();
        partyLevelDiffFactor08 = reader.Read<int32_t>();
        partyLevelDiffFactor09 = reader.Read<int32_t>();
        partyLevelDiffFactor10 = reader.Read<int32_t>();
        partyLevelDiffFactor11 = reader.Read<int32_t>();
        partyLevelDiffFactor12 = reader.Read<int32_t>();
        monsterLevelDiffFactor01 = reader.Read<int32_t>();
        monsterLevelDiffFactor02 = reader.Read<int32_t>();
        monsterLevelDiffFactor03 = reader.Read<int32_t>();
        monsterLevelDiffFactor04 = reader.Read<int32_t>();
        monsterLevelDiffFactor05 = reader.Read<int32_t>();
        monsterLevelDiffFactor06 = reader.Read<int32_t>();
        monsterLevelDiffFactor07 = reader.Read<int32_t>();
        monsterLevelDiffFactor08 = reader.Read<int32_t>();
        monsterLevelDiffFactor09 = reader.Read<int32_t>();
        monsterLevelDiffFactor10 = reader.Read<int32_t>();
        monsterLevelDiffFactor11 = reader.Read<int32_t>();
        monsterLevelDiffFactor12 = reader.Read<int32_t>();
        rareItemDiffFactor01 = reader.Read<int32_t>();
        rareItemDiffFactor02 = reader.Read<int32_t>();
        rareItemDiffFactor03 = reader.Read<int32_t>();
        rareItemDiffFactor04 = reader.Read<int32_t>();
        rareItemDiffFactor05 = reader.Read<int32_t>();
        rareItemDiffFactor06 = reader.Read<int32_t>();
        rareItemDiffFactor07 = reader.Read<int32_t>();
        rareItemDiffFactor08 = reader.Read<int32_t>();
        rareItemDiffFactor09 = reader.Read<int32_t>();
        rareItemDiffFactor10 = reader.Read<int32_t>();
        rareItemDiffFactor11 = reader.Read<int32_t>();
        rareItemDiffFactor12 = reader.Read<int32_t>();
    }

    void LevelDiffExpTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "LEVEL_DIFF_EXP.sox");
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

    auto LevelDiffExpTable::Find(int32_t index) const -> const LevelDiffExp*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto LevelDiffExpTable::Get() const -> const std::vector<LevelDiffExp>&
    {
        return _vector;
    }
}

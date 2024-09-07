#include "monster_item.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MonsterItem::MonsterItem(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        itemGenFrequency = reader.Read<int32_t>();
        itemsetGenFrequency = reader.Read<int32_t>();
        rareItemGenFrequency = reader.Read<int32_t>();
        rareItemsetGenFrequency = reader.Read<int32_t>();
        itemGoldMod = reader.Read<int32_t>();
        itemGoldMin = reader.Read<int32_t>();
        itemGoldMax = reader.Read<int32_t>();
        item01 = reader.Read<int32_t>();
        item01Mod = reader.Read<int32_t>();
        item02 = reader.Read<int32_t>();
        item02Mod = reader.Read<int32_t>();
        item03 = reader.Read<int32_t>();
        item03Mod = reader.Read<int32_t>();
        item04 = reader.Read<int32_t>();
        item04Mod = reader.Read<int32_t>();
        item05 = reader.Read<int32_t>();
        item05Mod = reader.Read<int32_t>();
        item06 = reader.Read<int32_t>();
        item06Mod = reader.Read<int32_t>();
        item07 = reader.Read<int32_t>();
        item07Mod = reader.Read<int32_t>();
        item08 = reader.Read<int32_t>();
        item08Mod = reader.Read<int32_t>();
        item09 = reader.Read<int32_t>();
        item09Mod = reader.Read<int32_t>();
        item10 = reader.Read<int32_t>();
        item10Mod = reader.Read<int32_t>();
        item11 = reader.Read<int32_t>();
        item11Mod = reader.Read<int32_t>();
        item12 = reader.Read<int32_t>();
        item12Mod = reader.Read<int32_t>();
        item13 = reader.Read<int32_t>();
        item13Mod = reader.Read<int32_t>();
        item14 = reader.Read<int32_t>();
        item14Mod = reader.Read<int32_t>();
        item15 = reader.Read<int32_t>();
        item15Mod = reader.Read<int32_t>();
        item16 = reader.Read<int32_t>();
        item16Mod = reader.Read<int32_t>();
        item17 = reader.Read<int32_t>();
        item17Mod = reader.Read<int32_t>();
        item18 = reader.Read<int32_t>();
        item18Mod = reader.Read<int32_t>();
        item19 = reader.Read<int32_t>();
        item19Mod = reader.Read<int32_t>();
        item20 = reader.Read<int32_t>();
        item20Mod = reader.Read<int32_t>();
        item21 = reader.Read<int32_t>();
        item21Mod = reader.Read<int32_t>();
        item22 = reader.Read<int32_t>();
        item22Mod = reader.Read<int32_t>();
        item23 = reader.Read<int32_t>();
        item23Mod = reader.Read<int32_t>();
        item24 = reader.Read<int32_t>();
        item24Mod = reader.Read<int32_t>();
        item25 = reader.Read<int32_t>();
        item25Mod = reader.Read<int32_t>();
        item26 = reader.Read<int32_t>();
        item26Mod = reader.Read<int32_t>();
        item27 = reader.Read<int32_t>();
        item27Mod = reader.Read<int32_t>();
        item28 = reader.Read<int32_t>();
        item28Mod = reader.Read<int32_t>();
        item29 = reader.Read<int32_t>();
        item29Mod = reader.Read<int32_t>();
        item30 = reader.Read<int32_t>();
        item30Mod = reader.Read<int32_t>();
        itemset01 = reader.Read<int32_t>();
        itemset01Mod = reader.Read<int32_t>();
        itemset02 = reader.Read<int32_t>();
        itemset02Mod = reader.Read<int32_t>();
        itemset03 = reader.Read<int32_t>();
        itemset03Mod = reader.Read<int32_t>();
        itemset04 = reader.Read<int32_t>();
        itemset04Mod = reader.Read<int32_t>();
        itemset05 = reader.Read<int32_t>();
        itemset05Mod = reader.Read<int32_t>();
        itemset06 = reader.Read<int32_t>();
        itemset06Mod = reader.Read<int32_t>();
        itemset07 = reader.Read<int32_t>();
        itemset07Mod = reader.Read<int32_t>();
        itemset08 = reader.Read<int32_t>();
        itemset08Mod = reader.Read<int32_t>();
        itemset09 = reader.Read<int32_t>();
        itemset09Mod = reader.Read<int32_t>();
        itemset10 = reader.Read<int32_t>();
        itemset10Mod = reader.Read<int32_t>();
        rareItem01 = reader.Read<int32_t>();
        rareItem01Mod = reader.Read<int32_t>();
        rareItem02 = reader.Read<int32_t>();
        rareItem02Mod = reader.Read<int32_t>();
        rareItem03 = reader.Read<int32_t>();
        rareItem03Mod = reader.Read<int32_t>();
        rareItem04 = reader.Read<int32_t>();
        rareItem04Mod = reader.Read<int32_t>();
        rareItem05 = reader.Read<int32_t>();
        rareItem05Mod = reader.Read<int32_t>();
        rareItem06 = reader.Read<int32_t>();
        rareItem06Mod = reader.Read<int32_t>();
        rareItem07 = reader.Read<int32_t>();
        rareItem07Mod = reader.Read<int32_t>();
        rareItem08 = reader.Read<int32_t>();
        rareItem08Mod = reader.Read<int32_t>();
        rareItem09 = reader.Read<int32_t>();
        rareItem09Mod = reader.Read<int32_t>();
        rareItem10 = reader.Read<int32_t>();
        rareItem10Mod = reader.Read<int32_t>();
        rareItemset01 = reader.Read<int32_t>();
        rareItemset01Mod = reader.Read<int32_t>();
        rareItemset02 = reader.Read<int32_t>();
        rareItemset02Mod = reader.Read<int32_t>();
        rareItemset03 = reader.Read<int32_t>();
        rareItemset03Mod = reader.Read<int32_t>();
        rareItemset04 = reader.Read<int32_t>();
        rareItemset04Mod = reader.Read<int32_t>();
        rareItemset05 = reader.Read<int32_t>();
        rareItemset05Mod = reader.Read<int32_t>();
        rareItemset06 = reader.Read<int32_t>();
        rareItemset06Mod = reader.Read<int32_t>();
        rareItemset07 = reader.Read<int32_t>();
        rareItemset07Mod = reader.Read<int32_t>();
        rareItemset08 = reader.Read<int32_t>();
        rareItemset08Mod = reader.Read<int32_t>();
        rareItemset09 = reader.Read<int32_t>();
        rareItemset09Mod = reader.Read<int32_t>();
        rareItemset10 = reader.Read<int32_t>();
        rareItemset10Mod = reader.Read<int32_t>();
    }

    void MonsterItemTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER_ITEM.sox");
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

    auto MonsterItemTable::Find(int32_t index) const -> const MonsterItem*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterItemTable::Get() const -> const std::vector<MonsterItem>&
    {
        return _vector;
    }
}

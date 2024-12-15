#include "item_set.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemSet::ItemSet(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
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
        item31 = reader.Read<int32_t>();
        item31Mod = reader.Read<int32_t>();
        item32 = reader.Read<int32_t>();
        item32Mod = reader.Read<int32_t>();
        item33 = reader.Read<int32_t>();
        item33Mod = reader.Read<int32_t>();
        item34 = reader.Read<int32_t>();
        item34Mod = reader.Read<int32_t>();
        item35 = reader.Read<int32_t>();
        item35Mod = reader.Read<int32_t>();
        item36 = reader.Read<int32_t>();
        item36Mod = reader.Read<int32_t>();
        item37 = reader.Read<int32_t>();
        item37Mod = reader.Read<int32_t>();
        item38 = reader.Read<int32_t>();
        item38Mod = reader.Read<int32_t>();
        item39 = reader.Read<int32_t>();
        item39Mod = reader.Read<int32_t>();
        item40 = reader.Read<int32_t>();
        item40Mod = reader.Read<int32_t>();
        item41 = reader.Read<int32_t>();
        item41Mod = reader.Read<int32_t>();
        item42 = reader.Read<int32_t>();
        item42Mod = reader.Read<int32_t>();
        item43 = reader.Read<int32_t>();
        item43Mod = reader.Read<int32_t>();
        item44 = reader.Read<int32_t>();
        item44Mod = reader.Read<int32_t>();
        item45 = reader.Read<int32_t>();
        item45Mod = reader.Read<int32_t>();
        item46 = reader.Read<int32_t>();
        item46Mod = reader.Read<int32_t>();
        item47 = reader.Read<int32_t>();
        item47Mod = reader.Read<int32_t>();
        item48 = reader.Read<int32_t>();
        item48Mod = reader.Read<int32_t>();
        item49 = reader.Read<int32_t>();
        item49Mod = reader.Read<int32_t>();
        item50 = reader.Read<int32_t>();
        item50Mod = reader.Read<int32_t>();
    }

    void ItemSetTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEM_SET.sox");
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

    auto ItemSetTable::Find(int32_t index) const -> const ItemSet*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemSetTable::Get() const -> const std::vector<ItemSet>&
    {
        return _vector;
    }
}

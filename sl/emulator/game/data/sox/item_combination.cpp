#include "item_combination.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemCombination::ItemCombination(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        result = reader.Read<int32_t>();
        job = reader.Read<int32_t>();
        level = reader.Read<int32_t>();
        ability = reader.Read<int32_t>();
        whenFails = reader.Read<int32_t>();
        failExp = reader.Read<int32_t>();
        orgMod1 = reader.Read<int32_t>();
        orgMod2 = reader.Read<int32_t>();
        req1 = reader.Read<int32_t>();
        req2 = reader.Read<int32_t>();
        req3 = reader.Read<int32_t>();
        req4 = reader.Read<int32_t>();
        req5 = reader.Read<int32_t>();
        req6 = reader.Read<int32_t>();
        req7 = reader.Read<int32_t>();
        req8 = reader.Read<int32_t>();
        req9 = reader.Read<int32_t>();
        req1num = reader.Read<int32_t>();
        req2num = reader.Read<int32_t>();
        req3num = reader.Read<int32_t>();
        req4num = reader.Read<int32_t>();
        req5num = reader.Read<int32_t>();
        req6num = reader.Read<int32_t>();
        req7num = reader.Read<int32_t>();
        req8num = reader.Read<int32_t>();
        req9num = reader.Read<int32_t>();
    }

    void ItemCombinationTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEM_COMBINATION.sox");
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

    auto ItemCombinationTable::Find(int32_t index) const -> const ItemCombination*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemCombinationTable::Get() const -> const std::vector<ItemCombination>&
    {
        return _vector;
    }
}

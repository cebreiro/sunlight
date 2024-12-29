#include "item_edible.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemEdible::ItemEdible(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        generate = reader.Read<int32_t>();
        explanation = reader.ReadString(reader.Read<uint16_t>());
        invenWidth = reader.Read<int32_t>();
        invenHeight = reader.Read<int32_t>();
        maxOverlapCount = reader.Read<int32_t>();
        rarity = reader.Read<int32_t>();
        price = reader.Read<int32_t>();
        modelID = reader.Read<int32_t>();
        modelColor = reader.Read<int32_t>();
        dropSound = reader.Read<int32_t>();
        useQuickSlot = reader.Read<int32_t>();
        oneMoreItem = reader.Read<int32_t>();
        equipPos = reader.Read<int32_t>();
        eatClass = reader.Read<int32_t>();
        effect1 = reader.Read<int32_t>();
        effectDuration1 = reader.Read<int32_t>();
        effectValue1 = reader.Read<int32_t>();
        effectPercent1 = reader.Read<int32_t>();
        effectReservedA1 = reader.Read<int32_t>();
        effectReservedB1 = reader.Read<int32_t>();
        effect2 = reader.Read<int32_t>();
        effectDuration2 = reader.Read<int32_t>();
        effectValue2 = reader.Read<int32_t>();
        effectPercent2 = reader.Read<int32_t>();
        effectReservedA2 = reader.Read<int32_t>();
        effectReservedB2 = reader.Read<int32_t>();
        effect3 = reader.Read<int32_t>();
        effectDuration3 = reader.Read<int32_t>();
        effectValue3 = reader.Read<int32_t>();
        effectPercent3 = reader.Read<int32_t>();
        effectReservedA3 = reader.Read<int32_t>();
        effectReservedB3 = reader.Read<int32_t>();
        effect4 = reader.Read<int32_t>();
        effectDuration4 = reader.Read<int32_t>();
        effectValue4 = reader.Read<int32_t>();
        effectPercent4 = reader.Read<int32_t>();
        effectReservedA4 = reader.Read<int32_t>();
        effectReservedB4 = reader.Read<int32_t>();
        effectSound = reader.Read<int32_t>();
        ableToSell = reader.Read<int32_t>();
        ableToTrade = reader.Read<int32_t>();
        ableToDrop = reader.Read<int32_t>();
        ableToDestroy = reader.Read<int32_t>();
        ableToStorage = reader.Read<int32_t>();
        conID = reader.Read<int32_t>();
        conRangeTime = reader.Read<int32_t>();
        reservedParam1 = reader.Read<int32_t>();
        reservedParam2 = reader.Read<int32_t>();
        reservedParam3 = reader.Read<int32_t>();
        reservedParam4 = reader.Read<int32_t>();
        reservedParam5 = reader.Read<int32_t>();
        reservedParam6 = reader.Read<int32_t>();
        reservedParam7 = reader.Read<int32_t>();
    }

    void ItemEdibleTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "item_edible.sox");
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

    auto ItemEdibleTable::Find(int32_t index) const -> const ItemEdible*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemEdibleTable::Get() const -> const std::vector<ItemEdible>&
    {
        return _vector;
    }
}

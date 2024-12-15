#include "item_clothes.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemClothes::ItemClothes(StreamReader<std::vector<char>::const_iterator>& reader)
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
        armorClass = reader.Read<int32_t>();
        defense = reader.Read<int32_t>();
        protection = reader.Read<int32_t>();
        equipEffect1 = reader.Read<int32_t>();
        equipEffectValue1 = reader.Read<int32_t>();
        equipEffect2 = reader.Read<int32_t>();
        equipEffectValue2 = reader.Read<int32_t>();
        equipEffect3 = reader.Read<int32_t>();
        equipEffectValue3 = reader.Read<int32_t>();
        equipEffect4 = reader.Read<int32_t>();
        equipEffectValue4 = reader.Read<int32_t>();
        constraint1 = reader.Read<int32_t>();
        constraint2 = reader.Read<int32_t>();
        constraint3 = reader.Read<int32_t>();
        constraint4 = reader.Read<int32_t>();
        constraint5 = reader.Read<int32_t>();
        constraint6 = reader.Read<int32_t>();
        constraint7 = reader.Read<int32_t>();
        constraint8 = reader.Read<int32_t>();
        constraintValue1 = reader.Read<int32_t>();
        constraintValue2 = reader.Read<int32_t>();
        constraintValue3 = reader.Read<int32_t>();
        constraintValue4 = reader.Read<int32_t>();
        constraintValue5 = reader.Read<int32_t>();
        constraintValue6 = reader.Read<int32_t>();
        constraintValue7 = reader.Read<int32_t>();
        constraintValue8 = reader.Read<int32_t>();
        constraintOR1 = reader.Read<int32_t>();
        constraintOR2 = reader.Read<int32_t>();
        constraintOR3 = reader.Read<int32_t>();
        constraintOR4 = reader.Read<int32_t>();
        constraintOR5 = reader.Read<int32_t>();
        constraintOR6 = reader.Read<int32_t>();
        constraintOR7 = reader.Read<int32_t>();
        ableToSell = reader.Read<int32_t>();
        ableToTrade = reader.Read<int32_t>();
        ableToDrop = reader.Read<int32_t>();
        ableToDestroy = reader.Read<int32_t>();
        ableToStorage = reader.Read<int32_t>();
    }

    void ItemClothesTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "item_clothes.sox");
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

    auto ItemClothesTable::Find(int32_t index) const -> const ItemClothes*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemClothesTable::Get() const -> const std::vector<ItemClothes>&
    {
        return _vector;
    }
}

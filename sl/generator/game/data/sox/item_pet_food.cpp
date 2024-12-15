#include "item_pet_food.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemPetFood::ItemPetFood(StreamReader<std::vector<char>::const_iterator>& reader)
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
        petExp = reader.Read<int32_t>();
        petNP = reader.Read<int32_t>();
        petId1 = reader.Read<int32_t>();
        petId2 = reader.Read<int32_t>();
        petId3 = reader.Read<int32_t>();
        ableToSell = reader.Read<int32_t>();
        ableToTrade = reader.Read<int32_t>();
        ableToDrop = reader.Read<int32_t>();
        ableToDestroy = reader.Read<int32_t>();
        ableToStorage = reader.Read<int32_t>();
    }

    void ItemPetFoodTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "item_pet_food.sox");
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

    auto ItemPetFoodTable::Find(int32_t index) const -> const ItemPetFood*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemPetFoodTable::Get() const -> const std::vector<ItemPetFood>&
    {
        return _vector;
    }
}

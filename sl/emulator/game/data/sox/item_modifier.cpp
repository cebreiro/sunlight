#include "item_modifier.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemModifier::ItemModifier(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        effect = reader.ReadString(reader.Read<uint16_t>());
        name = reader.ReadString(reader.Read<uint16_t>());
        generate = reader.Read<int32_t>();
        group = reader.Read<int32_t>();
        level = reader.Read<int32_t>();
        sortRank = reader.Read<int32_t>();
        rarity = reader.Read<int32_t>();
        addedTo = reader.Read<int32_t>();
        noModification = reader.Read<int32_t>();
        mod1 = reader.Read<int32_t>();
        mod1Min = reader.Read<float>();
        mod1Max = reader.Read<float>();
        mod1Type = reader.Read<int32_t>();
        mod1ExprType = reader.Read<int32_t>();
        mod1RoundingDigit = reader.Read<int32_t>();
        mod2 = reader.Read<int32_t>();
        mod2Min = reader.Read<float>();
        mod2Max = reader.Read<float>();
        mod2Type = reader.Read<int32_t>();
        mod2ExprType = reader.Read<int32_t>();
        mod2RoundingDigit = reader.Read<int32_t>();
        mod3 = reader.Read<int32_t>();
        mod3Min = reader.Read<float>();
        mod3Max = reader.Read<float>();
        mod3Type = reader.Read<int32_t>();
        mod3ExprType = reader.Read<int32_t>();
        mod3RoundingDigit = reader.Read<int32_t>();
        noConstraint = reader.Read<int32_t>();
        constr1 = reader.Read<int32_t>();
        constr1Value = reader.Read<int32_t>();
        constr2 = reader.Read<int32_t>();
        constr2Value = reader.Read<int32_t>();
        constr3 = reader.Read<int32_t>();
        constr3Value = reader.Read<int32_t>();
        constr4 = reader.Read<int32_t>();
        constr4Value = reader.Read<int32_t>();
    }

    void ItemModifierTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEM_MODIFIER.sox");
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

    auto ItemModifierTable::Find(int32_t index) const -> const ItemModifier*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemModifierTable::Get() const -> const std::vector<ItemModifier>&
    {
        return _vector;
    }
}

#include "item_base_attack.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemBaseAttack::ItemBaseAttack(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        motionCategory = reader.Read<int32_t>();
        dexBased = reader.Read<int32_t>();
        minimumDamage = reader.Read<int32_t>();
        maximumDamage = reader.Read<int32_t>();
        attackDelay = reader.Read<int32_t>();
        divDamage = reader.Read<int32_t>();
        divDamageDelay = reader.Read<int32_t>();
        combo = reader.Read<int32_t>();
        attackRange = reader.Read<int32_t>();
        hitMaterial = reader.Read<int32_t>();
        hitFX1 = reader.Read<int32_t>();
        hitFX2 = reader.Read<int32_t>();
        hitFX3 = reader.Read<int32_t>();
        hitFX4 = reader.Read<int32_t>();
        hitFX5 = reader.Read<int32_t>();
        hitFX6 = reader.Read<int32_t>();
        hitFX7 = reader.Read<int32_t>();
        hitFX8 = reader.Read<int32_t>();
        hitFX9 = reader.Read<int32_t>();
        hitFX10 = reader.Read<int32_t>();
    }

    void ItemBaseAttackTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEM_BASE_ATTACK.sox");
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

    auto ItemBaseAttackTable::Find(int32_t index) const -> const ItemBaseAttack*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemBaseAttackTable::Get() const -> const std::vector<ItemBaseAttack>&
    {
        return _vector;
    }
}

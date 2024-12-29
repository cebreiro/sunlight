#include "item_weapon.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemWeapon::ItemWeapon(StreamReader<std::vector<char>::const_iterator>& reader)
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
        weaponClass = reader.Read<int32_t>();
        motionCategory1 = reader.Read<int32_t>();
        jobMotion1 = reader.Read<int32_t>();
        motionCategory2 = reader.Read<int32_t>();
        jobMotion2 = reader.Read<int32_t>();
        motionCategory3 = reader.Read<int32_t>();
        jobMotion3 = reader.Read<int32_t>();
        damageType = reader.Read<int32_t>();
        attackType = reader.Read<int32_t>();
        dexBased = reader.Read<int32_t>();
        throwWeapon = reader.Read<int32_t>();
        minimumDamage = reader.Read<int32_t>();
        maximumDamage = reader.Read<int32_t>();
        attackDelay = reader.Read<int32_t>();
        divDamage = reader.Read<int32_t>();
        divDamageDelay = reader.Read<int32_t>();
        combo = reader.Read<int32_t>();
        attackRange = reader.Read<int32_t>();
        useBulletType = reader.Read<int32_t>();
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
        equipEffect1 = reader.Read<int32_t>();
        equipEffectValue1 = reader.Read<int32_t>();
        equipEffect2 = reader.Read<int32_t>();
        equipEffectValue2 = reader.Read<int32_t>();
        equipEffect3 = reader.Read<int32_t>();
        equipEffectValue3 = reader.Read<int32_t>();
        equipEffect4 = reader.Read<int32_t>();
        equipEffectValue4 = reader.Read<int32_t>();
        ableToSell = reader.Read<int32_t>();
        ableToTrade = reader.Read<int32_t>();
        ableToDrop = reader.Read<int32_t>();
        ableToDestroy = reader.Read<int32_t>();
        ableToStorage = reader.Read<int32_t>();
    }

    void ItemWeaponTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "item_weapon.sox");
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

    auto ItemWeaponTable::Find(int32_t index) const -> const ItemWeapon*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemWeaponTable::Get() const -> const std::vector<ItemWeapon>&
    {
        return _vector;
    }
}

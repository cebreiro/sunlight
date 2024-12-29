#include "conversion.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Conversion::Conversion(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        strPhysHit = reader.Read<int32_t>();
        strHp = reader.Read<int32_t>();
        dexHitAccuracy = reader.Read<int32_t>();
        dexEvade = reader.Read<int32_t>();
        dexPhysHit = reader.Read<int32_t>();
        constHp = reader.Read<int32_t>();
        constPhysDefense = reader.Read<int32_t>();
        intSp = reader.Read<int32_t>();
        intAbilityAttack = reader.Read<int32_t>();
        intAbilityAccuracy = reader.Read<int32_t>();
        willPhysDefense = reader.Read<int32_t>();
        willSp = reader.Read<int32_t>();
        willAbilityAttack = reader.Read<int32_t>();
        willAbilityAccuracy = reader.Read<int32_t>();
        elemAttack = reader.Read<int32_t>();
        elemDefense = reader.Read<int32_t>();
        rarityProbability = reader.Read<float>();
        rarityMoney = reader.Read<int32_t>();
        rarityLifetime = reader.Read<int32_t>();
        constHPRecoveryRate = reader.Read<float>();
        willSPRecoveryRate = reader.Read<float>();
        strNPDecreaseRate = reader.Read<float>();
        dexSpeed = reader.Read<int32_t>();
        dexAttackSpeed = reader.Read<int32_t>();
        dexAttackDelay = reader.Read<int32_t>();
        levelDiffExpRate = reader.Read<float>();
        levelFactorForHospital = reader.Read<float>();
        intCombination = reader.Read<int32_t>();
        rarityExp = reader.Read<int32_t>();
        levelExpPenalty = reader.Read<float>();
        willResist = reader.Read<float>();
        levelExpDistribution = reader.Read<int32_t>();
        levelAbnormalityProb = reader.Read<int32_t>();
    }

    void ConversionTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "CONVERSION.sox");
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

    auto ConversionTable::Find(int32_t index) const -> const Conversion*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ConversionTable::Get() const -> const std::vector<Conversion>&
    {
        return _vector;
    }
}

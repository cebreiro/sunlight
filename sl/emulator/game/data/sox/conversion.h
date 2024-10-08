#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct Conversion
    {
        explicit Conversion(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t strPhysHit = {};
        int32_t strHp = {};
        int32_t dexHitAccuracy = {};
        int32_t dexEvade = {};
        int32_t dexPhysHit = {};
        int32_t constHp = {};
        int32_t constPhysDefense = {};
        int32_t intSp = {};
        int32_t intAbilityAttack = {};
        int32_t intAbilityAccuracy = {};
        int32_t willPhysDefense = {};
        int32_t willSp = {};
        int32_t willAbilityAttack = {};
        int32_t willAbilityAccuracy = {};
        int32_t elemAttack = {};
        int32_t elemDefense = {};
        float rarityProbability = {};
        int32_t rarityMoney = {};
        int32_t rarityLifetime = {};
        float constHPRecoveryRate = {};
        float willSPRecoveryRate = {};
        float strNPDecreaseRate = {};
        int32_t dexSpeed = {};
        int32_t dexAttackSpeed = {};
        int32_t dexAttackDelay = {};
        float levelDiffExpRate = {};
        float levelFactorForHospital = {};
        int32_t intCombination = {};
        int32_t rarityExp = {};
        float levelExpPenalty = {};
        float willResist = {};
        int32_t levelExpDistribution = {};
        int32_t levelAbnormalityProb = {};
    };

    class ConversionTable final : public ISoxTable, public std::enable_shared_from_this<ConversionTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const Conversion*;
        auto Get() const -> const std::vector<Conversion>&;

    private:
        std::unordered_map<int32_t, Conversion*> _umap;
        std::vector<Conversion> _vector;

    };
}

#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct MonsterAction
    {
        explicit MonsterAction(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t modelID = {};
        int32_t bodySize = {};
        int32_t genFxType = {};
        int32_t sightRange = {};
        int32_t chaseRange = {};
        int32_t canGoOut = {};
        int32_t domain = {};
        int32_t characteristic = {};
        int32_t levelDiffMax = {};
        int32_t maxEnemy = {};
        int32_t scanTime = {};
        int32_t cancelTime = {};
        int32_t moveRangeMin = {};
        int32_t moveRangeMax = {};
        int32_t moveDelayMin = {};
        int32_t moveDelayMax = {};
        int32_t moveDelayChase = {};
        int32_t followship1 = {};
        int32_t followship2 = {};
        int32_t followship3 = {};
        int32_t followship4 = {};
        int32_t guardID = {};
        int32_t childID = {};
        int32_t childSummonDelay = {};
        int32_t childCount = {};
        int32_t childSummonFrame = {};
        int32_t hostileLenFactor = {};
        int32_t hostileHpFactor = {};
        int32_t hostileDefFactor = {};
        float hostileOtherAtt = {};
        int32_t chaseScore = {};
        int32_t memoryTime = {};
        int32_t aerialHeight = {};
        int32_t deadSound = {};
        int32_t moveSound = {};
    };

    class MonsterActionTable final : public ISoxTable, public std::enable_shared_from_this<MonsterActionTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const MonsterAction*;
        auto Get() const -> const std::vector<MonsterAction>&;

    private:
        std::unordered_map<int32_t, MonsterAction*> _umap;
        std::vector<MonsterAction> _vector;

    };
}

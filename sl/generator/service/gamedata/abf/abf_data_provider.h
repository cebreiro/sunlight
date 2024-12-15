#pragma once
#include "sl/data/abf/ability_file.h"

namespace sunlight
{
    struct AbilityRoutine;

    class AssetDataProvider;
    class SoxTableSet;
}

namespace sunlight
{
    class AbilityFileDataProvider
    {
    public:
        AbilityFileDataProvider(const std::filesystem::path& abfDirectory, const std::filesystem::path& monsterABFDirectory,
            const AssetDataProvider& assetDataProvider, const SoxTableSet& soxTableSet);

        auto Get(int32_t routineId) const -> const std::vector<AbilityRoutine>&;
        auto GetAbilityMonster(int32_t routineId) const -> const std::vector<AbilityRoutine>&;

    private:
        std::unordered_map<int32_t, AbilityFile> _abilityFiles;
        std::unordered_map<int32_t, AbilityFile> _monsterAbilityFiles;

        static const std::vector<AbilityRoutine> null_object;
    };
}

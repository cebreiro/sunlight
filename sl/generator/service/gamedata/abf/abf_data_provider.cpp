#include "abf_data_provider.h"

#include "sl/data/asset_data.h"
#include "sl/data/asset_data_provider.h"
#include "sl/generator/game/data/sox_table_set.h"
#include "sl/generator/game/data/sox/monster_skill.h"
#include "sl/generator/game/data/sox/skill_basic.h"

namespace sunlight
{
    const std::vector<AbilityRoutine> AbilityFileDataProvider::null_object;

    AbilityFileDataProvider::AbilityFileDataProvider(const std::filesystem::path& abfDirectory, const std::filesystem::path& monsterABFDirectory,
        const AssetDataProvider& assetDataProvider, const SoxTableSet& soxTableSet)
    {
        for (const sox::SkillBasic& soxSkillBasicData : soxTableSet.Get<sox::SkillBasicTable>().Get())
        {
            const AssetData* assetData = assetDataProvider.Find(soxSkillBasicData.routineId);
            if (!assetData)
            {
                continue;
            }

            const std::filesystem::path& filePath = abfDirectory / assetData->name;
            if (!exists(filePath))
            {
                continue;
            }

            const int32_t id = static_cast<int32_t>(assetData->id);

            if (_abilityFiles.contains(id))
            {
                continue;
            }

            const auto iter = _abilityFiles.try_emplace(id, AbilityFile::CreateFrom(filePath)).first;
            iter->second.assetId = id;
        }

        for (const sox::MonsterSkill& soxMonsterSkill : soxTableSet.Get<sox::MonsterSkillTable>().Get())
        {
            const AssetData* assetData = assetDataProvider.Find(soxMonsterSkill.routineId);
            if (!assetData)
            {
                continue;
            }

            const std::filesystem::path& filePath = monsterABFDirectory / assetData->name;
            if (!exists(filePath))
            {
                continue;
            }

            const int32_t id = static_cast<int32_t>(assetData->id);

            if (_monsterAbilityFiles.contains(id))
            {
                continue;
            }

            const auto iter = _monsterAbilityFiles.try_emplace(id, AbilityFile::CreateFrom(filePath)).first;
            iter->second.assetId = id;
        }
    }

    auto AbilityFileDataProvider::Get(int32_t routineId) const -> const std::vector<AbilityRoutine>&
    {
        const auto iter = _abilityFiles.find(routineId);

        return iter != _abilityFiles.end() ? iter->second.routines : null_object;
    }

    auto AbilityFileDataProvider::GetAbilityMonster(int32_t routineId) const -> const std::vector<AbilityRoutine>&
    {
        const auto iter = _monsterAbilityFiles.find(routineId);

        return iter != _monsterAbilityFiles.end() ? iter->second.routines : null_object;
    }
}

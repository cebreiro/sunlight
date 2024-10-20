#include "abf_data_provider.h"

#include <boost/algorithm/string.hpp>
#include "sl/data/asset_data.h"
#include "sl/data/asset_data_provider.h"
#include "sl/emulator/game/data/sox_table_set.h"
#include "sl/emulator/game/data/sox/skill_basic.h"

namespace sunlight
{
    AbilityFileDataProvider::AbilityFileDataProvider(const std::filesystem::path& assetDirectory,
        const AssetDataProvider& assetDataProvider, const SoxTableSet& soxTableSet)
    {
        for (const sox::SkillBasic& soxSkillBasicData : soxTableSet.Get<sox::SkillBasicTable>().Get())
        {
            const AssetData* assetData = assetDataProvider.Find(soxSkillBasicData.routineId);
            if (!assetData)
            {
                std::cout << fmt::format("fail to find routine asset. skill: {}, passive: {}\n", soxSkillBasicData.name, soxSkillBasicData.passive);

                continue;
            }

            const std::filesystem::path& filePath = assetDirectory / assetData->name;
            if (!exists(filePath))
            {
                std::cout << fmt::format("fail to find abf. skill: {}, passive: {}\n", soxSkillBasicData.name, soxSkillBasicData.passive);

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
    }

    auto AbilityFileDataProvider::Get(int32_t routineId) const -> const std::vector<AbilityRoutine>&
    {
        static const std::vector<AbilityRoutine> nullObject;

        const auto iter = _abilityFiles.find(routineId);

        return iter != _abilityFiles.end() ? iter->second.routines : nullObject;
    }
}

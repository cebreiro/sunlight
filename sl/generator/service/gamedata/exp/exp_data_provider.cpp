#include "exp_data_provider.h"

#include "sl/generator/game/data/sox/experience.h"
#include "sl/generator/game/data/sox_table_set.h"

namespace sunlight
{
    ExpDataProvider::ExpDataProvider(const SoxTableSet& tableSet)
    {
        for (const sox::Experience& data : tableSet.Get<sox::ExperienceTable>().Get())
        {
            _characterData[data.index] = CharacterExpData{
                .level = data.index,
                .expMax = data.cLvExp,
            };

            _jobData[data.index] = JobExpData{
                .level = data.index,
                .expMax = data.jLvExp,
            };
        }
    }

    auto ExpDataProvider::FindCharacterData(int32_t level) const -> const CharacterExpData*
    {
        const auto iter = _characterData.find(level);

        return iter != _characterData.end() ? &iter->second : nullptr;

    }

    auto ExpDataProvider::FindJobData(int32_t level) const -> const JobExpData*
    {
        const auto iter = _jobData.find(level);

        return iter != _jobData.end() ? &iter->second : nullptr;
    }
}

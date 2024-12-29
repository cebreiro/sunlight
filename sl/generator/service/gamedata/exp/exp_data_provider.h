#pragma once
#include "sl/generator/service/gamedata/exp/character_exp_data.h"
#include "sl/generator/service/gamedata/exp/job_exp_data.h"

namespace sunlight
{
    class SoxTableSet;
}

namespace sunlight
{
    class ExpDataProvider
    {
    public:
        explicit ExpDataProvider(const SoxTableSet& tableSet);

        auto FindCharacterData(int32_t level) const -> const CharacterExpData*;
        auto FindJobData(int32_t level) const -> const JobExpData*;

    private:
        std::unordered_map<int32_t, CharacterExpData> _characterData;
        std::unordered_map<int32_t, JobExpData> _jobData;
    };
}

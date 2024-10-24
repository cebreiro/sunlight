#pragma once
#include "sl/emulator/service/gamedata/skill/player_skill_data.h"

namespace sunlight
{
    class SoxTableSet;
    class AbilityFileDataProvider;
}

namespace sunlight
{
    class SkillDataProvider
    {
    public:
        SkillDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet, const AbilityFileDataProvider& abilityFileDataProvider);

        auto FindPlayerSkill(int32_t skillId) const -> const PlayerSkillData*;
        auto FindByJob(int32_t job, int32_t level) const -> const std::vector<PtrNotNull<const PlayerSkillData>>&;

    private:
        std::unordered_map<int32_t, PlayerSkillData> _playerSkillData;
        std::unordered_map<int32_t, std::unordered_map<int32_t, std::vector<PtrNotNull<const PlayerSkillData>>>> _jobIndex;
    };
}

#pragma once
#include "sl/generator/service/gamedata/skill/monster_skill_data.h"
#include "sl/generator/service/gamedata/skill/player_skill_data.h"

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

        auto FindMonsterSkill(int32_t skillId) const -> const MonsterSkillData*;

    private:
        std::unordered_map<int32_t, PlayerSkillData> _playerSkillData;
        std::unordered_map<int32_t, std::unordered_map<int32_t, std::vector<PtrNotNull<const PlayerSkillData>>>> _jobIndex;

        std::unordered_map<int32_t, MonsterSkillData> _monsterSkillData;
    };
}

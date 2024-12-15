#include "skill_data_provider.h"

#include "sl/generator/game/data/sox_table_set.h"
#include "sl/generator/game/data/sox/monster_skill.h"
#include "sl/generator/game/data/sox/skill_basic.h"
#include "sl/generator/service/gamedata/abf/abf_data_provider.h"

namespace sunlight
{
    SkillDataProvider::SkillDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet, const AbilityFileDataProvider& abilityFileDataProvider)
    {
        for (const sox::SkillBasic& soxData : tableSet.Get<sox::SkillBasicTable>().Get())
        {
            const auto [iter, inserted] = _playerSkillData.try_emplace(soxData.index, PlayerSkillData::CreateFrom(soxData));
            if (!inserted)
            {
                SUNLIGHT_LOG_ERROR(serviceLocator,
                    fmt::format("[{}] fail to insert player_skill_data. id: {}",
                        __FUNCTION__, soxData.index));
            }

            PlayerSkillData& data = iter->second;

            for (const AbilityRoutine& routine : abilityFileDataProvider.Get(data.routineId))
            {
                for (const AbilityProperty& property : routine.properties)
                {
                    for (const AbilityValue& value : property.values)
                    {
                        if (value.type == 2)
                        {
                            data.effectAttackValues[static_cast<WeaponClassType>(routine.weaponClass)].push_back(&value);
                        }
                    }
                }
            }

            for (const int32_t job : data.jobs)
            {
                _jobIndex[job][data.levelQualification].push_back(&data);
            }
        }

        for (const sox::MonsterSkill& soxMonsterSkill : tableSet.Get<sox::MonsterSkillTable>().Get())
        {
            MonsterSkillData monsterSkill = MonsterSkillData::CreateFrom(soxMonsterSkill);

            const auto [iter, inserted] = _monsterSkillData.try_emplace(soxMonsterSkill.index, MonsterSkillData::CreateFrom(soxMonsterSkill));
            if (!inserted)
            {
                continue;
            }

            MonsterSkillData& monsterSkillData = iter->second;

            const std::vector<AbilityRoutine>& monsterAbilityFileDataList = abilityFileDataProvider.GetAbilityMonster(monsterSkillData.routineId);
            assert(std::ssize(monsterAbilityFileDataList) <= 1);

            for (const AbilityRoutine& routine : monsterAbilityFileDataList)
            {
                for (const AbilityProperty& property : routine.properties)
                {
                    for (const AbilityValue& value : property.values)
                    {
                        if (value.type == 2)
                        {
                            monsterSkillData.effectAttackValues.push_back(&value);
                        }
                    }
                }
            }
        }
    }

    auto SkillDataProvider::FindPlayerSkill(int32_t skillId) const -> const PlayerSkillData*
    {
        const auto iter = _playerSkillData.find(skillId);

        return iter != _playerSkillData.end() ? &iter->second : nullptr;
    }

    auto SkillDataProvider::FindByJob(int32_t job, int32_t level) const -> const std::vector<PtrNotNull<const PlayerSkillData>>&
    {
        static const std::vector<PtrNotNull<const PlayerSkillData>> nullObject;

        const auto iter1 = _jobIndex.find(job);
        if (iter1 == _jobIndex.end())
        {
            return nullObject;
        }

        const auto iter2 = iter1->second.find(level);
        if (iter2 == iter1->second.end())
        {
            return nullObject;
        }

        return iter2->second;
    }

    auto SkillDataProvider::FindMonsterSkill(int32_t skillId) const -> const MonsterSkillData*
    {
        const auto iter = _monsterSkillData.find(skillId);

        return iter != _monsterSkillData.end() ? &iter->second : nullptr;
    }
}

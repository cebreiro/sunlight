#include "skill_data_provider.h"

#include "sl/emulator/game/data/sox_table_set.h"
#include "sl/emulator/game/data/sox/skill_basic.h"

namespace sunlight
{
    SkillDataProvider::SkillDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet)
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

            const PlayerSkillData& data = iter->second;

            for (const int32_t job : data.jobs)
            {
                _jobIndex[job][data.levelQualification].push_back(&data);
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
}

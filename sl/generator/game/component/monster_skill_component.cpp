#include "monster_skill_component.h"

#include "sl/generator/game/time/game_time_service.h"

namespace sunlight
{
    bool MonsterSkillComponent::CanUseSkill(int32_t skillId) const
    {
        const auto iter = _skillCoolTimeEnd.find(skillId);
        if (iter == _skillCoolTimeEnd.end())
        {
            return true;
        }

        return iter->second <= GameTimeService::Now();
    }

    void MonsterSkillComponent::SetCoolTime(int32_t skillId, game_time_point_type endTimePoint)
    {
        _skillCoolTimeEnd[skillId] = endTimePoint;
    }
}

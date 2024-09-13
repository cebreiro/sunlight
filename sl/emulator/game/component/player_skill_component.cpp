#include "player_skill_component.h"

namespace sunlight
{
    bool PlayerSkillComponent::AddSkill(const PlayerSkill& skill)
    {
        return _skills.try_emplace(skill.GetId(), skill).second;
    }

    auto PlayerSkillComponent::GetSkillCount() const -> int64_t
    {
        return std::ssize(_skills);
    }
}

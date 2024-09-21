#include "player_skill_component.h"

namespace sunlight
{
    bool PlayerSkillComponent::AddSkill(const PlayerSkill& skill)
    {
        return _skills.try_emplace(skill.GetId(), skill).second;
    }

    auto PlayerSkillComponent::FindSkill(int32_t skillId) -> PlayerSkill*
    {
        const auto iter = _skills.find(skillId);

        return iter != _skills.end() ? &iter->second : nullptr;
    }

    auto PlayerSkillComponent::FindSkill(int32_t skillId) const -> const PlayerSkill*
    {
        const auto iter = _skills.find(skillId);

        return iter != _skills.end() ? &iter->second : nullptr;
    }

    auto PlayerSkillComponent::GetSkillCount() const -> int64_t
    {
        return std::ssize(_skills);
    }
}

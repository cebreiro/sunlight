#include "player_skill_component.h"

#include "sl/generator/game/contents/passive/passive.h"

namespace sunlight
{
    bool PlayerSkillComponent::AddSkill(PlayerSkill skill)
    {
        const int32_t id = skill.GetId();

        return _skills.try_emplace(id, std::move(skill)).second;
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

#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/skill/player_skill.h"

namespace sunlight
{
    class PlayerSkillComponent final : public GameComponent
    {
    public:
        bool AddSkill(const PlayerSkill& skill);

        auto GetSkillCount() const -> int64_t;
        inline auto GetSkills() const;

    private:
        std::unordered_map<int32_t, PlayerSkill> _skills;
    };

    auto PlayerSkillComponent::GetSkills() const
    {
        return _skills | std::views::values;
    }
}

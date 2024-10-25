#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/time/game_time.h"

namespace sunlight
{
    class MonsterSkillComponent final : public GameComponent
    {
    public:
        bool CanUseSkill(int32_t skillId) const;

        void SetCoolTime(int32_t skillId, game_time_point_type endTimePoint);

    private:
        std::unordered_map<int32_t, game_time_point_type> _skillCoolTimeEnd;
    };
}

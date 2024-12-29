#pragma once
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_machine.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    class MonsterAIStateCombatFlee final : public MonsterAIStateMachine::state_type
    {
    public:
        MonsterAIStateCombatFlee();

        void OnEnter() override;
        auto OnEvent(const MonsterAIStateParam& event) -> Future<void> override;

    private:
        game_time_point_type _nextFleeTime = {};
    };
}

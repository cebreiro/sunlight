#pragma once
#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_machine.h"

namespace sunlight
{
    class MonsterAIStateCombat final : public MonsterAIStateMachine::state_type
    {
    public:
        MonsterAIStateCombat();

        auto OnEvent(const MonsterAIStateParam& event) -> Future<void> override;
    };
}

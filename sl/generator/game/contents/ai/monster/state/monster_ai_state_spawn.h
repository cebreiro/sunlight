#pragma once
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_machine.h"

namespace sunlight
{
    class MonsterAIStateSpawn final : public MonsterAIStateMachine::state_type
    {
    public:
        MonsterAIStateSpawn();

        auto OnEvent(const MonsterAIStateParam& event) -> Future<void> override;
    };
}

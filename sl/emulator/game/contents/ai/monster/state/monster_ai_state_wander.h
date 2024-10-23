#pragma once
#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_machine.h"

namespace sunlight
{
    class MonsterAIStateWander final : public MonsterAIStateMachine::state_type
    {
    public:
        MonsterAIStateWander();

        auto OnEvent(const MonsterAIStateParam& event) -> Future<void> override;

    private:
        std::mt19937 _mt;
    };
}

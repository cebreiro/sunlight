#pragma once
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_machine.h"

namespace sunlight
{
    class MonsterData;
}

namespace sunlight
{
    class MonsterAIStateFactory
    {
    public:
        MonsterAIStateFactory() = delete;

        static auto Create(const MonsterData& data, MonsterAIStateType type) -> std::shared_ptr<MonsterAIStateMachine::state_type>;
    };
}

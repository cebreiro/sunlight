#pragma once
#include "shared/state_machine/state_machine.h"
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state.h"

namespace sunlight
{
    class EntityAIControlSystem;
    class MonsterController;
    class GameMonster;
}

namespace sunlight
{
    class MonsterAIStateMachine;

    struct MonsterAIStateParam
    {
        EntityAIControlSystem& system;
        MonsterController& controller;
        GameMonster& monster;
        MonsterAIStateMachine& stateMachine;
    };

    class MonsterAIStateMachine final : public StateMachine<MonsterAIStateType, StateEvent<const MonsterAIStateParam&, Future<void>>>
    {
    public:
        using state_type = StateMachine::state_type;
    };
}

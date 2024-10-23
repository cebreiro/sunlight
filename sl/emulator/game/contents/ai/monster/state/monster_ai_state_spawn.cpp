#include "monster_ai_state_spawn.h"

namespace sunlight
{
    MonsterAIStateSpawn::MonsterAIStateSpawn()
        : IState(MonsterAIStateType::Spawn)
    {
    }

    auto MonsterAIStateSpawn::OnEvent(const MonsterAIStateParam& event) -> Future<void>
    {
        while (true)
        {
            co_await Delay(std::chrono::milliseconds(4000));

            if (event.stateMachine.Transition(MonsterAIStateType::Wander))
            {
                co_return;
            }
        }
    }
}

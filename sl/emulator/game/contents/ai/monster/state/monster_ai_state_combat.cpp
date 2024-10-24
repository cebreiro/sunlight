#include "monster_ai_state_combat.h"

namespace sunlight
{
    MonsterAIStateCombat::MonsterAIStateCombat()
        : IState(MonsterAIStateType::Combat)
    {
    }

    auto MonsterAIStateCombat::OnEvent(const MonsterAIStateParam& event) -> Future<void>
    {
        (void)event;

        co_return;
    }
}

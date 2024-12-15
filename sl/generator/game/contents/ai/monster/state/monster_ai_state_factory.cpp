#include "monster_ai_state_factory.h"

#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_combat.h"
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_combat_flee.h"
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_spawn.h"
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_wander.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    auto MonsterAIStateFactory::Create(const MonsterData& data, MonsterAIStateType type) -> std::shared_ptr<MonsterAIStateMachine::state_type>
    {
        (void)data;

        switch (type)
        {
        case MonsterAIStateType::Spawn:
        {
            return std::make_shared<MonsterAIStateSpawn>();
        }
        case MonsterAIStateType::Wander:
        {
            return std::make_shared<MonsterAIStateWander>();
        }
        case MonsterAIStateType::Combat:
        {
            if (data.GetAttack().type == 0)
            {
                return std::make_shared<MonsterAIStateCombatFlee>();
            }

            return std::make_shared<MonsterAIStateCombat>();
        }
        case MonsterAIStateType::Return:
            break;
        }

        return {};
    }
}

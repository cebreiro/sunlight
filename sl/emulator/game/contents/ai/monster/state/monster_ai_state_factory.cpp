#include "monster_ai_state_factory.h"

#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_spawn.h"
#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_wander.h"

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
            break;
        case MonsterAIStateType::Return:
            break;
        }

        return {};
    }
}

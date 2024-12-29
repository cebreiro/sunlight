#include "monster_ai_state_spawn.h"

#include "sl/generator/game/component/entity_state_component.h"
#include "sl/generator/game/contents/ai/monster/monster_controller.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/scene_object_message_creator.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"

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

            if (event.controller.ShouldStopCoroutine())
            {
                co_return;
            }

            GameMonster& monster = event.monster;

            EntityStateComponent& stateComponent = monster.GetStateComponent();
            stateComponent.SetState(GameEntityState{ .type = GameEntityStateType::Default, });

            event.system.Get<EntityViewRangeSystem>().VisitPlayer(monster, [&monster](GamePlayer& visited)
                {
                    visited.Send(SceneObjectPacketCreator::CreateState(monster));
                });

            if (event.stateMachine.Transition(MonsterAIStateType::Wander))
            {
                co_return;
            }
        }
    }
}

#include "monster_ai_state_wander.h"

#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/data/sox/monster_base.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/system/entity_ai_control_system.h"
#include "sl/emulator/game/system/entity_movement_system.h"
#include "sl/emulator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    MonsterAIStateWander::MonsterAIStateWander()
        : IState(MonsterAIStateType::Wander)
        , _mt(std::random_device{}())
    {
    }

    auto MonsterAIStateWander::OnEvent(const MonsterAIStateParam& event) -> Future<void>
    {
        EntityAIControlSystem& system = event.system;
        GameMonster& monster = event.monster;

        const SceneObjectComponent& sceneObjectComponent = monster.GetSceneObjectComponent();
        if (sceneObjectComponent.IsMoving())
        {
            co_return;
        }

        constexpr int32_t distance = 300;
        const double angle = std::uniform_int_distribution{ 0, 360 }(_mt) * std::numbers::pi / 180.0;

        Eigen::Vector2f destPos = sceneObjectComponent.GetPosition();
        destPos.x() += static_cast<float>(std::cos(angle) * distance);
        destPos.y() += static_cast<float>(std::sin(angle) * distance);

        const float speed = static_cast<float>(monster.GetData().GetBase().speed) / 100.f;

        system.Get<EntityMovementSystem>().MoveTo(monster, destPos, speed);
    }
}

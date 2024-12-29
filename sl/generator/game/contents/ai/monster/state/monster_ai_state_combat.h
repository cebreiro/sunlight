#pragma once
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_machine.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    class EntityAIControlSystem;

    class GameEntity;
    class GameMonster;
}
namespace sunlight
{
    class MonsterAIStateCombat final : public MonsterAIStateMachine::state_type
    {
    public:
        MonsterAIStateCombat();

        void OnEnter() override;
        auto OnEvent(const MonsterAIStateParam& event) -> Future<void> override;

    private:
        static auto GetPrimaryTarget(EntityAIControlSystem& system, GameMonster& monster) -> GameEntity*;

    private:
        std::optional<game_entity_id_type> _targetId = std::nullopt;
        std::optional<int32_t> _attackIndex = std::nullopt;
        game_time_point_type _nextChaseTimePoint = {};
    };
}

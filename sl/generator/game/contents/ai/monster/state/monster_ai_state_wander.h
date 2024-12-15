#pragma once
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_machine.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    class MonsterAIStateWander final : public MonsterAIStateMachine::state_type
    {
    public:
        MonsterAIStateWander();

        auto OnEvent(const MonsterAIStateParam& event) -> Future<void> override;

    private:
		game_time_point_type _nextMoveTimePoint = {};
        game_time_point_type _lastScanTime = {};
        std::vector<std::pair<game_entity_id_type, float>> _scanResult;
    };
}

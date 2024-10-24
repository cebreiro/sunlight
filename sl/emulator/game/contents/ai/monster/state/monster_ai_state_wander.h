#pragma once
#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_machine.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/time/game_time.h"

namespace sunlight
{
    class MonsterAIStateWander final : public MonsterAIStateMachine::state_type
    {
    public:
        MonsterAIStateWander();

        void OnEnter() override;
        auto OnEvent(const MonsterAIStateParam& event) -> Future<void> override;

    private:
        std::mt19937 _mt;

        game_time_point_type _lastScanTime = {};
        std::vector<std::pair<game_entity_id_type, float>> _playerScanResult;
    };
}

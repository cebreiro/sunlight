#pragma once
#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_machine.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class MonsterData;

    class EntityAIControlSystem;
}

namespace sunlight
{
    class MonsterController final : public std::enable_shared_from_this<MonsterController>
    {
    public:
        MonsterController(EntityAIControlSystem& system, game_entity_id_type entityId, const MonsterData& data);
        ~MonsterController();

        bool ShouldStopCoroutine() const;
        void ConfigureCoroutineExecutionContext();

        void Start();
        void Shutdown();

        void Suspend();
        void Resume();

    public:
        auto GetData() const -> const MonsterData&;

        void SetTickInterval(std::chrono::milliseconds interval);

    private:
        auto Run() -> Future<void>;

    private:
        EntityAIControlSystem& _system;
        game_entity_id_type _entityId;
        const MonsterData& _data;

        bool _shutdown = false;
        SharedPtrNotNull<future::SharedContext<void>> _suspendContext;

        std::chrono::milliseconds _tickInterval = std::chrono::milliseconds(200);

        MonsterAIStateMachine _stateMachine;
    };
}

#include "monster_controller.h"

#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_machine.h"
#include "sl/emulator/game/contents/ai/monster/state/monster_ai_state_factory.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/system/entity_ai_control_system.h"
#include "sl/emulator/game/system/scene_object_system.h"

namespace sunlight
{
    MonsterController::MonsterController(EntityAIControlSystem& system, game_entity_id_type entityId, const MonsterData& data)
        : _system(system)
        , _entityId(entityId)
        , _data(data)
    {
        auto spawnState = MonsterAIStateFactory::Create(data, MonsterAIStateType::Spawn);
        assert(spawnState);

        _stateMachine.AddState(true, std::move(spawnState));

        if (auto wanderState = MonsterAIStateFactory::Create(data, MonsterAIStateType::Wander);
            wanderState)
        {
            _stateMachine.AddState(false, std::move(wanderState));

            _stateMachine.GetTransition(MonsterAIStateType::Spawn)->Add(MonsterAIStateType::Wander);
        }

        if (auto combatState = MonsterAIStateFactory::Create(data, MonsterAIStateType::Combat);
            combatState)
        {
            _stateMachine.AddState(false, std::move(combatState));

            if (MonsterAIStateMachine::StateTransition* transition = _stateMachine.GetTransition(MonsterAIStateType::Wander);
                transition)
            {
                transition->Add(MonsterAIStateType::Combat);

                _stateMachine.GetTransition(MonsterAIStateType::Combat)->Add(MonsterAIStateType::Wander);
            }
        }
    }

    MonsterController::~MonsterController()
    {
    }

    void MonsterController::Start()
    {
        Post(*ExecutionContext::GetExecutor(), [self = shared_from_this()]()
            {
                self->Run();
            });
    }

    void MonsterController::Shutdown()
    {
        if (std::exchange(_shutdown, true) == false)
        {
            if (_suspendContext)
            {
                _suspendContext->OnSuccess();
            }
        }
    }

    void MonsterController::Suspend()
    {
        if (!_suspendContext)
        {
            _suspendContext = std::make_shared<future::SharedContext<void>>();
        }

        _suspendContext->Reset();
    }

    void MonsterController::Resume()
    {
        if (!_suspendContext)
        {
            return;
        }

        _suspendContext->OnSuccess();
    }

    auto MonsterController::GetData() const -> const MonsterData&
    {
        return _data;
    }

    void MonsterController::SetTickInterval(std::chrono::milliseconds interval)
    {
        _tickInterval = interval;
    }

    auto MonsterController::Run() -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        while (true)
        {
            co_await Delay(_tickInterval);

            if (_shutdown)
            {
                co_return;
            }

            if (_suspendContext && _suspendContext->IsPending())
            {
                co_await Future(_suspendContext);

                if (_shutdown)
                {
                    co_return;
                }
            }

            const auto& entity = _system.Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, _entityId);
            if (!entity)
            {
                co_return;
            }

            const MonsterAIStateParam param{
                .system = _system,
                .controller = *this,
                .monster = *entity->Cast<GameMonster>(),
                .stateMachine = _stateMachine,
            };

            co_await _stateMachine.OnEvent(param);

            if (_shutdown)
            {
                co_return;
            }
        }
    }
}

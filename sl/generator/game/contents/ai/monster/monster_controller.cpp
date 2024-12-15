#include "monster_controller.h"

#include "sl/generator/game/component/monster_stat_component.h"
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_machine.h"
#include "sl/generator/game/contents/ai/monster/state/monster_ai_state_factory.h"
#include "sl/generator/game/debug/game_debugger.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/zone_execution_environment.h"

namespace sunlight
{
    MonsterController::MonsterController(EntityAIControlSystem& system, game_entity_id_type entityId, const MonsterData& data)
        : _system(system)
        , _entityId(entityId)
        , _data(data)
        , _mt(std::random_device{}())
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

    bool MonsterController::IsRunning() const
    {
        if (_suspendContext && _suspendContext->IsPending())
        {
            return false;
        }

        return true;
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

    bool MonsterController::ShouldStopCoroutine() const
    {
        const GameEntity* entity = _system.Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, _entityId);
        if (!entity)
        {
            return true;
        }

        if (entity->GetId().GetRecycleSequence() != _entityId.GetRecycleSequence())
        {
            return true;
        }

        const GameMonster& monster = *entity->Cast<GameMonster>();
        if (monster.IsDead())
        {
            return true;
        }

        return false;
    }

    void MonsterController::ConfigureCoroutineExecutionContext()
    {
        const auto now = game_clock_type::now();
        GameTimeService::SetNow(now);
    }

    auto MonsterController::Rand(int32_t min, int32_t max) -> int32_t
    {
        if (min > max)
        {
            assert(false);

            std::swap(min, max);
        }
        else if (min == max)
        {
            assert(false);

            return min;
        }

        std::uniform_int_distribution dist(min, max);

        return dist(_mt);
    }

    auto MonsterController::GetData() const -> const MonsterData&
    {
        return _data;
    }

    auto MonsterController::GetRandomEngine() -> std::mt19937&
    {
        return _mt;
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

            std::optional<ZoneExecutionEnvironment> environment(_system.GetServiceLocator());

            GameEntity* entity = _system.Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, _entityId);
            if (!entity)
            {
                co_return;
            }

            GameMonster& monster = *entity->Cast<GameMonster>();
            if (monster.IsDead())
            {
                co_return;
            }

            const MonsterAIStateParam param{
                .system = _system,
                .controller = *this,
                .monster = monster,
                .stateMachine = _stateMachine,
            };

            Future<void> future = _stateMachine.OnEvent(param);
            environment.reset();

            if (future.IsPending())
            {
                co_await future;
            }

            if (_shutdown)
            {
                co_return;
            }
        }
    }
}

#include "zone_timer_service.h"

#include "sl/emulator/game/debug/game_debugger.h"
#include "sl/emulator/game/entity/game_entity.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/zone.h"

namespace sunlight
{
    ZoneTimerService::ZoneTimerService(Zone& zone)
        : _zone(zone)
    {
    }

    auto ZoneTimerService::GetName() const -> std::string_view
    {
        return "zone_timer_service";
    }

    void ZoneTimerService::AddTimer(std::chrono::milliseconds delay, const std::function<void()>& function)
    {
        Delay(delay).Then(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), function]()
            {
                const std::shared_ptr<Zone> zone = weak.lock();
                if (!zone)
                {
                    return;
                }

                if (GameDebugger* debugger = zone->GetServiceLocator().Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
                {
                    GameDebugger::SetInstance(debugger);
                }

                GameTimeService::SetNow(game_clock_type::now());

                function();

                GameDebugger::SetInstance(nullptr);
            });
    }

    void ZoneTimerService::AddTimer(std::chrono::milliseconds delay, const GameEntity& entity, int32_t stageId, const std::function<void(GameEntity&)>& function)
    {
        Delay(delay).Then(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), type = entity.GetType(), id = entity.GetId(), stageId, function]()
            {
                const std::shared_ptr<Zone> zone = weak.lock();
                if (!zone)
                {
                    return;
                }

                Stage* stage = zone->FindStage(stageId);
                if (!stage)
                {
                    return;
                }

                const auto& entity = stage->Get<SceneObjectSystem>().FindEntity(type, id);
                if (!entity)
                {
                    return;
                }

                if (GameDebugger* debugger = zone->GetServiceLocator().Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
                {
                    GameDebugger::SetInstance(debugger);
                }

                GameTimeService::SetNow(game_clock_type::now());

                function(*entity);

                GameDebugger::SetInstance(nullptr);
            });
    }

    void ZoneTimerService::AddTimer(std::chrono::milliseconds delay, int64_t playerId, int32_t stageId, const std::function<void(GamePlayer&)>& function)
    {
        Delay(delay).Then(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), playerId, stageId, function]()
            {
                const std::shared_ptr<Zone> zone = weak.lock();
                if (!zone)
                {
                    return;
                }

                Stage* stage = zone->FindPlayerStage(playerId);
                if (!stage || stage->GetId() != stageId)
                {
                    return;
                }

                GamePlayer* player = stage->Get<PlayerIndexSystem>().FindByCId(playerId);
                if (!player)
                {
                    return;
                }

                if (GameDebugger* debugger = zone->GetServiceLocator().Find<GameDebugger>(); debugger && debugger->HasDebugTarget())
                {
                    GameDebugger::SetInstance(debugger);
                }

                GameTimeService::SetNow(game_clock_type::now());

                function(*player);

                GameDebugger::SetInstance(nullptr);
            });
    }
}

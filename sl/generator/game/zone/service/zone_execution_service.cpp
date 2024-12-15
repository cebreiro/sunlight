#include "zone_execution_service.h"

#include "sl/generator/game/entity/game_entity.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/system/player_index_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/zone.h"
#include "sl/generator/game/zone/zone_execution_environment.h"

namespace sunlight
{
    ZoneExecutionService::ZoneExecutionService(Zone& zone)
        : _zone(zone)
    {
    }

    auto ZoneExecutionService::GetName() const -> std::string_view
    {
        return "zone_execution_service";
    }

    void ZoneExecutionService::Post(const std::function<void()>& function, const std::source_location& current)
    {
        execution::Post(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), function, current]()
            {
                const std::shared_ptr<Zone> zone = weak.lock();
                if (!zone)
                {
                    return;
                }

                ZoneExecutionEnvironment environment(zone->GetServiceLocator(), current);

                function();
            });
    }

    void ZoneExecutionService::Post(game_entity_id_type id, int32_t stageId, const std::function<void(GameMonster&)>& function, const std::source_location& current)
    {
        execution::Post(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), id, stageId, function, current]()
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

                GameEntity* entity = stage->Get<SceneObjectSystem>().FindEntity(id);
                if (!entity || entity->GetId().GetRecycleSequence() != id.GetRecycleSequence())
                {
                    return;
                }

                if (entity->GetType() != GameMonster::TYPE)
                {
                    assert(false);

                    return;
                }

                ZoneExecutionEnvironment environment(zone->GetServiceLocator(), current);

                function(*entity->Cast<GameMonster>());
            });
    }

    void ZoneExecutionService::Post(int64_t playerId, int32_t stageId, const std::function<void(GamePlayer&)>& function, const std::source_location& current)
    {
        execution::Post(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), playerId, stageId, function, current]()
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

                ZoneExecutionEnvironment environment(zone->GetServiceLocator(), current);

                function(*player);
            });
    }

    void ZoneExecutionService::AddTimer(std::chrono::milliseconds delay, const std::function<void()>& function,
        const std::source_location& current)
    {
        Delay(delay).Then(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), function, current]()
            {
                const std::shared_ptr<Zone> zone = weak.lock();
                if (!zone)
                {
                    return;
                }

                ZoneExecutionEnvironment environment(zone->GetServiceLocator(), current);

                function();
            });
    }

    void ZoneExecutionService::AddTimer(std::chrono::milliseconds delay, const GameEntity& entity, int32_t stageId,
        const std::function<void(GameEntity&)>& function, const std::source_location& current)
    {
        Delay(delay).Then(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), type = entity.GetType(), id = entity.GetId(), stageId, function, current]()
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

                GameEntity* entity = stage->Get<SceneObjectSystem>().FindEntity(type, id);
                if (!entity || entity->GetId().GetRecycleSequence() != id.GetRecycleSequence())
                {
                    return;
                }

                ZoneExecutionEnvironment environment(zone->GetServiceLocator(), current);

                function(*entity);
            });
    }

    void ZoneExecutionService::AddTimer(std::chrono::milliseconds delay, int64_t playerId, int32_t stageId,
        const std::function<void(GamePlayer&)>& function, const std::source_location& current)
    {
        Delay(delay).Then(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), playerId, stageId, function, current]()
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

                ZoneExecutionEnvironment environment(zone->GetServiceLocator(), current);

                function(*player);
            });
    }
}

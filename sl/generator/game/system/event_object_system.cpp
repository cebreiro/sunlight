#include "event_object_system.h"

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/event_object_stage_entrance_portal_component.h"
#include "sl/generator/game/component/event_object_zone_portal_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/entity/game_event_object.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/event_object_spawner_component.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/event_bubbling/monster_event_bubbling.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/zone_change_service.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/map/map_data_provider.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"
#include "sl/generator/service/game_cheat_log/game_cheat_log_service.h"

namespace sunlight
{
    EventObjectSystem::EventObjectSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
    {
    }

    void EventObjectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());

        if (PathFindingSystem* pathFindSystem = stage.Find<PathFindingSystem>();
            pathFindSystem)
        {
            Add(*pathFindSystem);
        }
    }

    bool EventObjectSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::TRIGGER,
            std::bind_front(&EventObjectSystem::HandleTrigger, this)))
        {
            return false;
        }

        stage.Get<EventBubblingSystem>().AddSubscriber<EventBubblingMonsterDespawn>(
            [this](const EventBubblingMonsterDespawn& event)
            {
                this->HandleMonsterDespawn(event);
            });


        return true;
    }

    auto EventObjectSystem::GetName() const -> std::string_view
    {
        return "event_object_system";
    }

    auto EventObjectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EventObjectSystem>();
    }

    bool EventObjectSystem::AddEventObject(SharedPtrNotNull<GameEventObject> eventObject)
    {
        const game_entity_id_type id = eventObject->GetId();

        if (EventObjectSpawnerComponent* spawnerComponent = eventObject->FindComponent<EventObjectSpawnerComponent>();
            spawnerComponent)
        {
            ZoneExecutionService& zoneExecutionService = _serviceLocator.Get<ZoneExecutionService>();

            for (SpawnerContext& context : spawnerComponent->GetSpawnDataRange())
            {
                context.timerRunning = true;

                zoneExecutionService.AddTimer(std::chrono::milliseconds(context.firstDelay),
                    [this, id = id, mobId = context.data->GetId()]()
                    {
                        this->OnExpireSpawnerTimer(id, mobId);
                    });
            }
        }

        return _eventObjects.try_emplace(id, std::move(eventObject)).second;
    }

    auto EventObjectSystem::FindEventObject(game_entity_id_type id) -> GameEventObject*
    {
        const auto iter = _eventObjects.find(id);

        return iter != _eventObjects.end() ? iter->second.get() : nullptr;
    }

    auto EventObjectSystem::FindEventObject(game_entity_id_type id) const -> const GameEventObject*
    {
        const auto iter = _eventObjects.find(id);

        return iter != _eventObjects.end() ? iter->second.get() : nullptr;
    }

    void EventObjectSystem::OnExpireSpawnerTimer(game_entity_id_type id, int32_t mobId)
    {
        GameEventObject* eventObject = FindEventObject(id);
        assert(eventObject);

        EventObjectSpawnerComponent& spawnerComponent = eventObject->GetComponent<EventObjectSpawnerComponent>();
        SpawnerContext* spawnContext = spawnerComponent.FindContext(mobId);
        assert(spawnContext);

        ++spawnContext->spawnCount;

        const Eigen::Vector2f spawnPos = [&]() -> Eigen::Vector2f
            {
                Eigen::Vector2f randPos;

                if (PathFindingSystem* pathFindSystem = Find<PathFindingSystem>();
                    pathFindSystem && pathFindSystem->GetRandPositionInBox(eventObject->GetArea(), randPos))
                {
                    return randPos;
                }

                return eventObject->GetCenterPosition();
            }();

        GameMonsterSpawnerContext spawnerContext{
            .spawnerId = eventObject->GetId(),
            .spawnerCenter = eventObject->GetCenterPosition(),
            .spawnerArea = eventObject->GetArea(),
        };

        auto monster = std::make_shared<GameMonster>(_serviceLocator.Get<GameEntityIdPublisher>(), *spawnContext->data, spawnPos, spawnerContext);

        Get<SceneObjectSystem>().SpawnMonster(std::move(monster), spawnPos, 0.f);

        if (spawnContext->spawnCount < spawnContext->maxCount)
        {
            _serviceLocator.Get<ZoneExecutionService>().AddTimer(std::chrono::milliseconds(spawnContext->delay),
                [this, id, mobId]()
                {
                    OnExpireSpawnerTimer(id, mobId);
                });
        }
        else
        {
            spawnContext->timerRunning = false;
        }
    }

    void EventObjectSystem::HandleMonsterDespawn(const EventBubblingMonsterDespawn& event)
    {
        const GameMonster& monster = *event.monster;

        if (const std::optional<GameMonsterSpawnerContext>& spawnerContext = event.monster->GetSpawnerContext();
            spawnerContext.has_value())
        {
            [[maybe_unused]]
            bool success = false;

            do
            {
                GameEventObject* eventObject = FindEventObject(spawnerContext->spawnerId);
                if (!eventObject)
                {
                    break;
                }

                EventObjectSpawnerComponent* spawnerComponent = eventObject->FindComponent<EventObjectSpawnerComponent>();
                if (!spawnerComponent)
                {
                    break;
                }

                SpawnerContext* spawnContext = spawnerComponent->FindContext(monster.GetDataId());
                if (!spawnContext)
                {
                    break;
                }

                --spawnContext->spawnCount;

                if (!spawnContext->timerRunning)
                {
                    spawnContext->timerRunning = true;

                    _serviceLocator.Get<ZoneExecutionService>().AddTimer(std::chrono::milliseconds(spawnContext->delay),
                        [this, id = eventObject->GetId(), mobId = monster.GetDataId()]()
                        {
                            OnExpireSpawnerTimer(id, mobId);
                        });
                }

                success = true;
                
            } while (false);

            assert(success);
        }
    }

    void EventObjectSystem::HandleTrigger(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;

        do
        {
            if (message.targetType != GameEntityType::EventObj)
            {
                break;
            }

            const GameEventObject* eventObject = FindEventObject(message.targetId);
            if (!eventObject)
            {
                break;
            }

            const SceneObjectComponent& sceneObjectComponent = player.GetSceneObjectComponent();
            const Eigen::Vector2f& playerPosition = sceneObjectComponent.GetPosition();
            const Eigen::AlignedBox2f& areaBox = eventObject->GetArea();

            const Eigen::Vector2f closetPoint = playerPosition.cwiseMax(areaBox.min()).cwiseMin(areaBox.max());

            const float radius = static_cast<float>(sceneObjectComponent.GetBodySize()) + GameConstant::AABB_COLLISION_NETWORK_DELAY_MARGIN;
            const float distanceSq = (closetPoint - playerPosition).squaredNorm();

            if (distanceSq >= (radius * radius))
            {
                _serviceLocator.Get<GameCheatLogService>().Log(GameCheatLogType::Trigger, player.GetName(),
                    fmt::format("trigger distance. allow: {}, distance: {}", radius, std::sqrt(distanceSq)));
            }

            if (const auto* stageEntranceComponent  = eventObject->FindComponent<EventObjectStageEntrancePortalComponent>();
                stageEntranceComponent)
            {
                _serviceLocator.Get<ZoneChangeService>().StartStageChange(player, _stageId, stageEntranceComponent->GetLinkId());
            }
            else if (const auto* zonePortalComponent = eventObject->FindComponent<EventObjectZonePortalComponent>();
                zonePortalComponent)
            {
                const int32_t destZoneId = zonePortalComponent->GetDestZoneId();
                const int32_t linkId = zonePortalComponent->GetLinkId();
                const MapDataProvider& mapDataProvider = _serviceLocator.Find<GameDataProvideService>()->GetMapDataProvider();

                if (const MapGateData* data = mapDataProvider.FindMapGateData(destZoneId, linkId);
                    data)
                {
                    const int32_t destX = static_cast<int32_t>(data->centerPosition.x());
                    const int32_t destY = static_cast<int32_t>(data->centerPosition.y());

                    _serviceLocator.Get<ZoneChangeService>().StartZoneChange(player.GetClientId(), destZoneId, destX, destY);
                }
            }

            return;

        } while (false);

        SUNLIGHT_LOG_WARN(_serviceLocator,
            fmt::format("[{}] fail to handle trigger. id: {}, type: {}, player: {}",
                GetName(), message.targetId, ToString(message.targetType), message.player.GetCId()));
    }
}

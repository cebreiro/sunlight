#include "event_object_system.h"

#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/event_object_stage_entrance_portal_component.h"
#include "sl/emulator/game/component/event_object_zone_portal_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/entity/game_event_object.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/zone_change_service.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/map/map_data_provider.h"

namespace sunlight
{
    EventObjectSystem::EventObjectSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
    {
    }

    void EventObjectSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool EventObjectSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::TRIGGER,
            std::bind_front(&EventObjectSystem::HandleTrigger, this)))
        {
            return false;
        }

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

            if ((closetPoint - playerPosition).squaredNorm() >= (radius * radius))
            {
                // TODO: allow and cheat log

                break;
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

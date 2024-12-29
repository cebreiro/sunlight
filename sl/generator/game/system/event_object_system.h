#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;
    struct EventBubblingMonsterDespawn;

    class GameEventObject;
}

namespace sunlight
{
    class EventObjectSystem final : public GameSystem
    {
    public:
        EventObjectSystem(const ServiceLocator& serviceLocator, int32_t stageId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool AddEventObject(SharedPtrNotNull<GameEventObject> eventObject);

        auto FindEventObject(game_entity_id_type id) -> GameEventObject*;
        auto FindEventObject(game_entity_id_type id) const -> const GameEventObject*;

    private:
        void OnExpireSpawnerTimer(game_entity_id_type id, int32_t mobId);
        void HandleMonsterDespawn(const EventBubblingMonsterDespawn& event);

        void HandleTrigger(const ZoneMessage& message);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<GameEventObject>> _eventObjects;
    };
}

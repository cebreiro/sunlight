#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    struct EventBubblingMonsterSpawn;
    struct EventBubblingMonsterDespawn;

    class MonsterController;
}

namespace sunlight
{
    class EntityAIControlSystem final : public GameSystem
    {
    public:
        explicit EntityAIControlSystem(const ServiceLocator& serviceLocator);
        ~EntityAIControlSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        auto GetServiceLocator() const -> const ServiceLocator&;

    public:
        bool SuspendMonsterControl(game_entity_id_type id);
        bool ResumeMonsterControl(game_entity_id_type id);

    private:
        auto FindMonsterController(game_entity_id_type id) -> MonsterController*;
        auto FindMonsterController(game_entity_id_type id) const -> const MonsterController*;

    private:
        void HandleEvent(const EventBubblingMonsterSpawn& e);
        void HandleEvent(const EventBubblingMonsterDespawn& e);

    private:
        const ServiceLocator& _serviceLocator;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<MonsterController>> _monsterControllers;
    };
}

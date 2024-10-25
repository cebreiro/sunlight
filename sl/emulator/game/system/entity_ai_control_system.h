#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct EventBubblingMonsterAIAttach;
    struct EventBubblingMonsterAIDetach;

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

    private:
        void HandleEvent(const EventBubblingMonsterAIAttach& e);
        void HandleEvent(const EventBubblingMonsterAIDetach& e);

    private:
        const ServiceLocator& _serviceLocator;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<MonsterController>> _monsterControllers;
    };
}

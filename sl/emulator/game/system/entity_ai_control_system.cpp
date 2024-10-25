#include "entity_ai_control_system.h"

#include "sl/emulator/game/contents/ai/monster/monster_controller.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/system/entity_movement_system.h"
#include "sl/emulator/game/system/entity_scan_system.h"
#include "sl/emulator/game/system/event_bubbling_system.h"
#include "sl/emulator/game/system/monster_skill_effect_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/system/event_bubbling/ai_event_bubbling.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    EntityAIControlSystem::EntityAIControlSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    EntityAIControlSystem::~EntityAIControlSystem()
    {
    }

    void EntityAIControlSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityMovementSystem>());
        Add(stage.Get<EntityScanSystem>());
        Add(stage.Get<MonsterSkillEffectSystem>());
    }

    bool EntityAIControlSystem::Subscribe(Stage& stage)
    {
        EventBubblingSystem& eventBubblingSystem = stage.Get<EventBubblingSystem>();

        eventBubblingSystem.AddSubscriber<EventBubblingMonsterAIAttach>(
            [this](const EventBubblingMonsterAIAttach& e)
            {
                HandleEvent(e);
            });

        eventBubblingSystem.AddSubscriber<EventBubblingMonsterAIDetach>(
            [this](const EventBubblingMonsterAIDetach& e)
            {
                HandleEvent(e);
            });

        return true;
    }

    auto EntityAIControlSystem::GetName() const -> std::string_view
    {
        return "ai_control_system";
    }

    auto EntityAIControlSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityAIControlSystem>();
    }

    auto EntityAIControlSystem::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    void EntityAIControlSystem::HandleEvent(const EventBubblingMonsterAIAttach& e)
    {
        assert(!_monsterControllers.contains(e.entityId));

        const GameEntity* entity = Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, e.entityId);
        if (!entity)
        {
            assert(false);

            return;
        }

        auto controller = std::make_shared<MonsterController>(*this, e.entityId, entity->Cast<GameMonster>()->GetData());
        controller->Start();

        _monsterControllers[e.entityId] = std::move(controller);
    }

    void EntityAIControlSystem::HandleEvent(const EventBubblingMonsterAIDetach& e)
    {
        const auto iter = _monsterControllers.find(e.entityId);
        if (iter == _monsterControllers.end())
        {
            return;
        }

        MonsterController& monsterController = *iter->second;
        monsterController.Shutdown();

        _monsterControllers.erase(iter);
    }
}

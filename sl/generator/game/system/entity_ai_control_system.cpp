#include "entity_ai_control_system.h"

#include "sl/generator/game/contents/ai/monster/monster_controller.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/entity_scan_system.h"
#include "sl/generator/game/system/entity_skill_effect_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/event_bubbling/monster_event_bubbling.h"
#include "sl/generator/game/zone/stage.h"

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
        Add(stage.Get<EntitySkillEffectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());

        if (PathFindingSystem* pathFindSystem = stage.Find<PathFindingSystem>();
            pathFindSystem)
        {
            Add(*pathFindSystem);
        }
    }

    bool EntityAIControlSystem::Subscribe(Stage& stage)
    {
        EventBubblingSystem& eventBubblingSystem = stage.Get<EventBubblingSystem>();

        eventBubblingSystem.AddSubscriber<EventBubblingMonsterSpawn>(
            [this](const EventBubblingMonsterSpawn& e)
            {
                HandleEvent(e);
            });

        eventBubblingSystem.AddSubscriber<EventBubblingMonsterDespawn>(
            [this](const EventBubblingMonsterDespawn& e)
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

    bool EntityAIControlSystem::SuspendMonsterControl(game_entity_id_type id)
    {
        MonsterController* controller = FindMonsterController(id);
        if (!controller)
        {
            return false;
        }

        controller->Suspend();

        return true;
    }

    bool EntityAIControlSystem::ResumeMonsterControl(game_entity_id_type id)
    {
        MonsterController* controller = FindMonsterController(id);
        if (!controller)
        {
            return false;
        }

        controller->Resume();

        return true;
    }

    auto EntityAIControlSystem::FindMonsterController(game_entity_id_type id) -> MonsterController*
    {
        const auto iter = _monsterControllers.find(id);

        return iter != _monsterControllers.end() ? iter->second.get() : nullptr;
    }

    auto EntityAIControlSystem::FindMonsterController(game_entity_id_type id) const -> const MonsterController*
    {
        const auto iter = _monsterControllers.find(id);

        return iter != _monsterControllers.end() ? iter->second.get() : nullptr;
    }

    void EntityAIControlSystem::HandleEvent(const EventBubblingMonsterSpawn& e)
    {
        assert(!_monsterControllers.contains(e.monster->GetId()));

        GameMonster& monster = *e.monster;

        auto controller = std::make_shared<MonsterController>(*this, monster.GetId(), monster.GetData());
        controller->Start();

        _monsterControllers[monster.GetId()] = std::move(controller);
    }

    void EntityAIControlSystem::HandleEvent(const EventBubblingMonsterDespawn& e)
    {
        const auto iter = _monsterControllers.find(e.monster->GetId());
        if (iter == _monsterControllers.end())
        {
            return;
        }

        MonsterController& monsterController = *iter->second;
        monsterController.Shutdown();

        _monsterControllers.erase(iter);
    }
}

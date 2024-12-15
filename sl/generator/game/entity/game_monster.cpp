#include "game_monster.h"

#include "sl/generator/game/component/entity_immune_component.h"
#include "sl/generator/game/component/entity_movement_component.h"
#include "sl/generator/game/component/entity_state_component.h"
#include "sl/generator/game/component/entity_status_effect_component.h"
#include "sl/generator/game/component/monster_aggro_component.h"
#include "sl/generator/game/component/monster_skill_component.h"
#include "sl/generator/game/component/monster_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/data/sox/monster_action.h"
#include "sl/generator/game/data/sox/monster_base.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    GameMonster::GameMonster(GameEntityIdPublisher& idPublisher, const MonsterData& data, Eigen::Vector2f spawnPosition,
        const std::optional<GameMonsterSpawnerContext>& spawnContext)
        : GameEntity(idPublisher, TYPE)
        , _data(data)
        , _spawnPosition(std::move(spawnPosition))
        , _spawnerContext(spawnContext)
    {
        AddComponent(std::make_unique<SceneObjectComponent>());
        AddComponent(std::make_unique<EntityStateComponent>());
        AddComponent(std::make_unique<EntityStatusEffectComponent>());
        AddComponent(std::make_unique<MonsterStatComponent>(_data.GetBase()));
        AddComponent(std::make_unique<EntityMovementComponent>());
        AddComponent(std::make_unique<MonsterAggroComponent>(_data.GetAction()));
        AddComponent(std::make_unique<MonsterSkillComponent>());
        AddComponent(std::make_unique<EntityImmuneComponent>());

        GetSceneObjectComponent().SetBodySize(data.GetAction().bodySize);
    }

    bool GameMonster::IsInvisible() const
    {
        return false;
    }

    bool GameMonster::IsDead() const
    {
        return _dead;
    }

    auto GameMonster::GetDataId() const -> int32_t
    {
        return _data.GetId();
    }

    auto GameMonster::GetData() const -> const MonsterData&
    {
        return _data;
    }

    auto GameMonster::GetSpawnPosition() const -> Eigen::Vector2f
    {
        return _spawnPosition;
    }

    auto GameMonster::GetSpawnerContext() const -> const std::optional<GameMonsterSpawnerContext>&
    {
        return _spawnerContext;
    }

    auto GameMonster::GetMoveSpeed() const -> float
    {
        return static_cast<float>(GetData().GetBase().speed) / 100.f;
    }

    auto GameMonster::GetChaseSpeed() const -> float
    {
        return static_cast<float>(GetData().GetBase().speedChase) / 100.f;
    }

    void GameMonster::SetDead(bool value)
    {
        _dead = value;
    }

    auto GameMonster::GetSceneObjectComponent() -> SceneObjectComponent&
    {
        return GetComponent<SceneObjectComponent>();
    }

    auto GameMonster::GetSceneObjectComponent() const -> const SceneObjectComponent&
    {
        return GetComponent<SceneObjectComponent>();
    }

    auto GameMonster::GetStateComponent() -> EntityStateComponent&
    {
        return GetComponent<EntityStateComponent>();
    }

    auto GameMonster::GetStateComponent() const -> const EntityStateComponent&
    {
        return GetComponent<EntityStateComponent>();
    }

    auto GameMonster::GetStatusEffectComponent() -> EntityStatusEffectComponent&
    {
        return GetComponent<EntityStatusEffectComponent>();
    }

    auto GameMonster::GetStatusEffectComponent() const -> const EntityStatusEffectComponent&
    {
        return GetComponent<EntityStatusEffectComponent>();
    }

    auto GameMonster::GetMovementComponent() -> EntityMovementComponent&
    {
        return GetComponent<EntityMovementComponent>();
    }

    auto GameMonster::GetMovementComponent() const -> const EntityMovementComponent&
    {
        return GetComponent<EntityMovementComponent>();
    }

    auto GameMonster::GetStatComponent() -> MonsterStatComponent&
    {
        return GetComponent<MonsterStatComponent>();
    }

    auto GameMonster::GetStatComponent() const -> const MonsterStatComponent&
    {
        return GetComponent<MonsterStatComponent>();
    }

    auto GameMonster::GetAggroComponent() -> MonsterAggroComponent&
    {
        return GetComponent<MonsterAggroComponent>();
    }

    auto GameMonster::GetAggroComponent() const -> const MonsterAggroComponent&
    {
        return GetComponent<MonsterAggroComponent>();
    }

    auto GameMonster::GetSkillComponent() -> MonsterSkillComponent&
    {
        return GetComponent<MonsterSkillComponent>();
    }

    auto GameMonster::GetSkillComponent() const -> const MonsterSkillComponent&
    {
        return GetComponent<MonsterSkillComponent>();
    }

    auto GameMonster::GetImmuneComponent() -> EntityImmuneComponent&
    {
        return GetComponent<EntityImmuneComponent>();
    }

    auto GameMonster::GetImmuneComponent() const -> const EntityImmuneComponent&
    {
        return GetComponent<EntityImmuneComponent>();
    }
}

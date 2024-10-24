#include "game_monster.h"

#include "sl/emulator/game/component/entity_movement_component.h"
#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/entity_status_effect_component.h"
#include "sl/emulator/game/component/monster_aggro_component.h"
#include "sl/emulator/game/component/monster_stat_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/data/sox/monster_action.h"
#include "sl/emulator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    GameMonster::GameMonster(GameEntityIdPublisher& idPublisher, const MonsterData& data, std::optional<game_entity_id_type> spawnerId)
        : GameEntity(idPublisher, TYPE)
        , _data(data)
        , _spawnerId(spawnerId)
    {
        AddComponent(std::make_unique<SceneObjectComponent>());
        AddComponent(std::make_unique<EntityStateComponent>());
        AddComponent(std::make_unique<EntityStatusEffectComponent>());
        AddComponent(std::make_unique<MonsterStatComponent>(_data.GetBase()));
        AddComponent(std::make_unique<EntityMovementComponent>());
        AddComponent(std::make_unique<MonsterAggroComponent>(_data.GetAction()));

        GetSceneObjectComponent().SetBodySize(data.GetAction().bodySize);
    }

    bool GameMonster::IsInvisible() const
    {
        return false;
    }

    auto GameMonster::GetDataId() const -> int32_t
    {
        return _data.GetId();
    }

    auto GameMonster::GetData() const -> const MonsterData&
    {
        return _data;
    }

    auto GameMonster::GetSpawnerId() const -> std::optional<game_entity_id_type>
    {
        return _spawnerId;
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
}

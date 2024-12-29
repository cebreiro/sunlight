#pragma once
#include "sl/generator/game/entity/game_entity.h"

namespace sunlight
{
    class MonsterData;

    class SceneObjectComponent;
    class EntityStateComponent;
    class EntityStatusEffectComponent;
    class EntityMovementComponent;
    class MonsterStatComponent;
    class MonsterAggroComponent;
    class MonsterSkillComponent;
    class EntityImmuneComponent;
}

namespace sunlight
{
    struct GameMonsterSpawnerContext
    {
        game_entity_id_type spawnerId = {};
        Eigen::Vector2f spawnerCenter = {};
        Eigen::AlignedBox2f spawnerArea = {};
    };

    class GameMonster final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::Enemy;

    public:
        GameMonster(GameEntityIdPublisher& idPublisher, const MonsterData& data,
            Eigen::Vector2f spawnPosition, const std::optional<GameMonsterSpawnerContext>& spawnContext);

        bool IsInvisible() const;
        bool IsDead() const;

        auto GetDataId() const -> int32_t;
        auto GetData() const -> const MonsterData&;
        auto GetSpawnPosition() const -> Eigen::Vector2f;
        auto GetSpawnerContext() const -> const std::optional<GameMonsterSpawnerContext>&;

        auto GetMoveSpeed() const -> float;
        auto GetChaseSpeed() const -> float;

        void SetDead(bool value);

    public:
        auto GetSceneObjectComponent() -> SceneObjectComponent&;
        auto GetSceneObjectComponent() const -> const SceneObjectComponent&;
        auto GetStateComponent() -> EntityStateComponent&;
        auto GetStateComponent() const -> const EntityStateComponent&;
        auto GetStatusEffectComponent() -> EntityStatusEffectComponent&;
        auto GetStatusEffectComponent() const -> const EntityStatusEffectComponent&;
        auto GetMovementComponent() -> EntityMovementComponent&;
        auto GetMovementComponent() const -> const EntityMovementComponent&;
        auto GetStatComponent() -> MonsterStatComponent&;
        auto GetStatComponent() const -> const MonsterStatComponent&;
        auto GetAggroComponent() -> MonsterAggroComponent&;
        auto GetAggroComponent() const -> const MonsterAggroComponent&;
        auto GetSkillComponent() -> MonsterSkillComponent&;
        auto GetSkillComponent() const -> const MonsterSkillComponent&;
        auto GetImmuneComponent() -> EntityImmuneComponent&;
        auto GetImmuneComponent() const -> const EntityImmuneComponent&;

    private:
        const MonsterData& _data;
        Eigen::Vector2f _spawnPosition;
        std::optional<GameMonsterSpawnerContext> _spawnerContext = std::nullopt;

        bool _dead = false;
    };
}

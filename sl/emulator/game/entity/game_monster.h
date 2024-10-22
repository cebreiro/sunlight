#pragma once
#include "sl/emulator/game/entity/game_entity.h"

namespace sunlight
{
    class MonsterData;

    class SceneObjectComponent;
    class EntityStateComponent;
    class MonsterStatComponent;
}

namespace sunlight
{
    class GameMonster final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::Enemy;

    public:
        GameMonster(GameEntityIdPublisher& idPublisher, const MonsterData& data, std::optional<game_entity_id_type> spawnerId);

        bool IsInvisible() const;

        auto GetDataId() const -> int32_t;
        auto GetData() const -> const MonsterData&;
        auto GetSpawnerId() const -> std::optional<game_entity_id_type>;

    public:
        auto GetSceneObjectComponent() -> SceneObjectComponent&;
        auto GetSceneObjectComponent() const -> const SceneObjectComponent&;
        auto GetStateComponent() -> EntityStateComponent&;
        auto GetStateComponent() const -> const EntityStateComponent&;
        auto GetStatComponent() -> MonsterStatComponent&;
        auto GetStatComponent() const -> const MonsterStatComponent&;

    private:
        const MonsterData& _data;

        std::optional<game_entity_id_type> _spawnerId = std::nullopt;
    };
}

#pragma once
#include "sl/emulator/game/entity/game_entity.h"

namespace sunlight
{
    class MonsterData;
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

    private:
        const MonsterData& _data;

        std::optional<game_entity_id_type> _spawnerId = std::nullopt;
    };
}

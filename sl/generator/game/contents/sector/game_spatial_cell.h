#pragma once
#include "sl/generator/game/contents/sector/game_spatial_id.h"
#include "sl/generator/game/contents/sector/game_spatial_mbr.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"

namespace sunlight
{
    class GameEntity;
}

namespace sunlight
{
    class GameSpatialCell
    {
    public:
        GameSpatialCell(game_spatial_cell_id_type id, const GameSpatialMBR& mbr);

        bool Empty(GameEntityType type) const;
        bool Contains(GameEntityType type, game_entity_id_type id) const;

        void AddEntity(GameEntity& entity);
        void RemoveEntity(GameEntity& entity);

        auto GetId() const -> game_spatial_cell_id_type;
        auto GetMBR() const -> const GameSpatialMBR&;
        auto GetQuadIndex(const Eigen::Vector2d& position) const -> int64_t;

        auto GetSize() const -> int64_t;
        auto GetSize(GameEntityType type) const -> int64_t;

        inline auto GetEntities(GameEntityType type) const;
        inline auto GetEntities() const;

    private:
        auto GetContainer(GameEntityType type) const -> const std::vector<PtrNotNull<GameEntity>>&;

    private:
        game_spatial_cell_id_type _id;
        GameSpatialMBR _mbr;
        double _halfX = 0.0;
        double _halfY = 0.0;

        std::unordered_map<GameEntityType, std::vector<PtrNotNull<GameEntity>>> _entities;
    };

    inline auto GameSpatialCell::GetEntities(GameEntityType type) const
    {
        return GetContainer(type) | notnull::reference;
    }

    inline auto GameSpatialCell::GetEntities() const
    {
        return _entities | std::views::values | std::views::join | notnull::reference;
    }
}

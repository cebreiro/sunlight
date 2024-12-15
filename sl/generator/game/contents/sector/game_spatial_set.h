#pragma once
#include <boost/container/static_vector.hpp>

#include "sl/generator/game/contents/sector/game_spatial_cell.h"

namespace sunlight
{
    class GameSpatialCell;
}

namespace sunlight
{
    class GameSpatialSet
    {
    public:
        virtual ~GameSpatialSet() = default;

        bool Empty() const;

        bool HasEntitiesAtLeast(int64_t count) const;
        bool HasEntitiesAtLeast(GameEntityType type, int64_t count) const;
        bool HasCell(game_spatial_cell_id_type id) const;

        virtual void AddCell(PtrNotNull<GameSpatialCell> cell);

        inline auto GetCells() const;
        inline auto GetEntities() const;
        inline auto GetEntities(GameEntityType type) const;

    protected:
        static bool CellCompare(PtrNotNull<const GameSpatialCell> lhs, PtrNotNull<const GameSpatialCell> rhs);

    protected:
        boost::container::static_vector<PtrNotNull<GameSpatialCell>, 9> _cells;
    };

    auto GameSpatialSet::GetCells() const
    {
        return _cells | notnull::reference;
    }

    auto GameSpatialSet::GetEntities() const
    {
        return _cells
            | std::views::transform([](PtrNotNull<const GameSpatialCell> cell)
                {
                    return cell->GetEntities();
                })
            | std::views::join;
    }

    auto GameSpatialSet::GetEntities(GameEntityType type) const
    {
        return _cells
            | std::views::transform([type](PtrNotNull<const GameSpatialCell> cell)
                {
                    return cell->GetEntities(type);
                })
            | std::views::join;
    }
}

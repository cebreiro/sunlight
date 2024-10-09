#include "game_spatial_set.h"

namespace sunlight
{
    bool GameSpatialSet::Empty() const
    {
        return _cells.empty();
    }

    bool GameSpatialSet::HasEntitiesAtLeast(int64_t count) const
    {
        int64_t sum = 0;

        for (PtrNotNull<GameSpatialCell> cell : _cells)
        {
            sum += cell->GetSize();

            if (sum >= count)
            {
                return true;
            }
        }

        return false;
    }

    bool GameSpatialSet::HasEntitiesAtLeast(GameEntityType type, int64_t count) const
    {
        int64_t sum = 0;

        for (PtrNotNull<GameSpatialCell> cell : _cells)
        {
            sum += cell->GetSize(type);

            if (sum >= count)
            {
                return true;
            }
        }

        return false;
    }

    bool GameSpatialSet::HasCell(game_spatial_cell_id_type id) const
    {
        const auto iter = std::ranges::find_if(_cells, [id](PtrNotNull<GameSpatialCell> cell) -> bool
            {
                return cell->GetId() == id;
            });

        return iter != _cells.end();
    }

    void GameSpatialSet::AddCell(PtrNotNull<GameSpatialCell> cell)
    {
        assert(!HasCell(cell->GetId()));
        assert(_cells.size() + 1 <= _cells.max_size());

        auto iter = std::ranges::lower_bound(_cells, cell, CellCompare);
        _cells.insert(iter, cell);
    }

    bool GameSpatialSet::CellCompare(PtrNotNull<const GameSpatialCell> lhs, PtrNotNull<const GameSpatialCell> rhs)
    {
        return lhs->GetId() < rhs->GetId();
    }
}

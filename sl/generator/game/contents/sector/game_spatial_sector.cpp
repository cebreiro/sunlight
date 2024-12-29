#include "game_spatial_sector.h"

namespace sunlight
{
    GameSpatialSector::GameSpatialSector(game_spatial_sector_id_type id)
        : _id(id)
    {
    }

    void GameSpatialSector::AddCell(PtrNotNull<GameSpatialCell> cell)
    {
        GameSpatialSet::AddCell(cell);

        if (_center)
        {
            return;
        }

        if (const game_spatial_cell_id_type cellID = cell->GetId();
            cellID.GetX() == _id.GetX() && cell->GetId().GetY() == _id.GetY())
        {
            _center = cell;
        }
    }

    void GameSpatialSector::AddEntity(GameEntity& entity)
    {
        assert(_center);

        _center->AddEntity(entity);
    }

    void GameSpatialSector::RemoveEntity(GameEntity& entity)
    {
        assert(_center);

        _center->RemoveEntity(entity);
    }

    auto GameSpatialSector::Difference(const GameSpatialSector& other) const -> Subset
    {
        Subset view;
        std::ranges::set_difference(_cells, other._cells,
            std::back_inserter(view._cells), CellCompare);

        return view;
    }

    auto GameSpatialSector::Intersect(const GameSpatialSector& other) const -> Subset
    {
        Subset view;
        std::ranges::set_intersection(_cells, other._cells,
            std::back_inserter(view._cells), CellCompare);

        return view;
    }

    auto GameSpatialSector::GetId() const -> game_spatial_sector_id_type
    {
        return _id;
    }

    auto GameSpatialSector::GetCenter() const -> const GameSpatialCell&
    {
        return *_center;
    }

    auto operator-(const GameSpatialSector& lhs, const GameSpatialSector& rhs) -> GameSpatialSector::Subset
    {
        return lhs.Difference(rhs);
    }

    GameSpatialSector::Subset operator&(const GameSpatialSector& lhs, const GameSpatialSector& rhs)
    {
        return lhs.Intersect(rhs);
    }
}

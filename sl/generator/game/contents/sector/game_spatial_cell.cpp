#include "game_spatial_cell.h"

#include "sl/generator/game/entity/game_entity.h"

namespace sunlight
{
    GameSpatialCell::GameSpatialCell(game_spatial_cell_id_type id, const GameSpatialMBR& mbr)
        : _id(id)
        , _mbr(mbr)
        , _halfX((_mbr.GetMin().x() + _mbr.GetMax().x()) / 2.0)
        , _halfY((_mbr.GetMin().y() + _mbr.GetMax().y()) / 2.0)
    {
    }

    bool GameSpatialCell::Empty(GameEntityType type) const
    {
        return GetContainer(type).empty();
    }

    bool GameSpatialCell::Contains(GameEntityType type, game_entity_id_type id) const
    {
        const std::vector<PtrNotNull<GameEntity>>& container = GetContainer(type);

        return std::ranges::find_if(container, [id](PtrNotNull<GameEntity> entity)
            {
                return entity->GetId() == id;
            }) != container.end();
    }

    void GameSpatialCell::AddEntity(GameEntity& entity)
    {
        std::vector<GameEntity*>& container = _entities[entity.GetType()];
        assert(!std::ranges::contains(container, &entity));

        container.push_back(&entity);
    }

    void GameSpatialCell::RemoveEntity(GameEntity& entity)
    {
        const auto iter1 = _entities.find(entity.GetType());
        if (iter1 == _entities.end())
        {
            assert(false);

            return;
        }

        const auto iter2 = std::ranges::find(iter1->second, &entity);
        if (iter2 == iter1->second.end())
        {
            assert(false);

            return;
        }

        iter1->second.erase(iter2);
    }

    auto GameSpatialCell::GetId() const -> game_spatial_cell_id_type
    {
        return _id;
    }

    auto GameSpatialCell::GetMBR() const -> const GameSpatialMBR&
    {
        return _mbr;
    }

    auto GameSpatialCell::GetQuadIndex(const Eigen::Vector2d& position) const -> int64_t
    {
        assert(position.x() >= _mbr.GetMin().x() && position.x() <= _mbr.GetMax().x());
        assert(position.y() >= _mbr.GetMin().y() && position.y() <= _mbr.GetMax().y());

        // quad index
        // ----------- X
        // | 0  | 1  |
        // | 2  | 3  |
        // -----------
        // Y

        int64_t value = position.y() <= _halfY ? 0 : 2;
        value += position.x() <= _halfX ? 0 : 1;

        return value;
    }

    auto GameSpatialCell::GetSize() const -> int64_t
    {
        return std::accumulate(_entities.begin(), _entities.end(), int64_t{ 0 }, [](int64_t sum, const auto& pair)
            {
                return sum + std::ssize(pair.second);
            });
    }

    auto GameSpatialCell::GetSize(GameEntityType type) const -> int64_t
    {
        return std::ssize(GetContainer(type));
    }

    auto GameSpatialCell::GetContainer(GameEntityType type) const -> const std::vector<PtrNotNull<GameEntity>>&
    {
        static std::vector<PtrNotNull<GameEntity>> nullObject;

        const auto iter = _entities.find(type);
        if (iter == _entities.end())
        {
            return nullObject;
        }

        return iter->second;
    }
}

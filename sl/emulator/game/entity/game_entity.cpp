#include "game_entity.h"

namespace sunlight
{
    GameEntity::GameEntity(game_entity_id_type id, GameEntityType type)
        : _id(id)
        , _type(type)
    {
    }

    bool GameEntity::IsActive() const
    {
        return _active;
    }

    auto GameEntity::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    auto GameEntity::GetType() const -> GameEntityType
    {
        return _type;
    }

    void GameEntity::SetActive(bool value)
    {
        _active = value;
    }
}

#include "game_entity.h"

#include "sl/generator/game/zone/service/game_entity_id_publisher.h"

namespace sunlight
{
    GameEntity::GameEntity(GameEntityIdPublisher& idPublisher, GameEntityType type)
        : _idPublisher(&idPublisher)
        , _id(idPublisher.Publish(type))
        , _type(type)
    {
    }

    GameEntity::GameEntity(game_entity_id_type id, GameEntityType type)
        : _id(id)
        , _type(type)
    {
    }

    GameEntity::~GameEntity()
    {
        if (_idPublisher)
        {
            _idPublisher->Return(_type, _id);
        }
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

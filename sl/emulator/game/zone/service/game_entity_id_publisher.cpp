#include "game_entity_id_publisher.h"

namespace sunlight
{
    GameEntityIdPublisher::GameEntityIdPublisher(int32_t zoneId)
        : _name(fmt::format("game_entity_id_publisher_{}", zoneId))
    {
    }

    auto GameEntityIdPublisher::GetName() const -> std::string_view
    {
        return _name;
    }

    auto GameEntityIdPublisher::Publish(GameEntityType type) -> game_entity_id_type
    {
        (void)type;

        game_entity_id_type result = game_entity_id_type(NULL_ID);

        if (_recycleQueue.empty())
        {
            while (result == game_entity_id_type(NULL_ID))
            {
                result = game_entity_id_type(++_nextValue);
            }
        }
        else
        {
            result = _recycleQueue.front();
            _recycleQueue.pop();
        }

        return result;
    }

    void GameEntityIdPublisher::Return(GameEntityType type, game_entity_id_type id)
    {
        (void)type;

        _recycleQueue.push(id);
    }

    auto GameEntityIdPublisher::PublishSceneObjectId() -> int32_t
    {
        int32_t result = 0;

        if (_recycleQueueSceneObjectId.empty())
        {
            while (result == 0)
            {
                result = ++_nextSceneObjectId;
            }
        }
        else
        {
            result = _recycleQueueSceneObjectId.front();
            _recycleQueueSceneObjectId.pop();
        }

        return result;
    }

    void GameEntityIdPublisher::ReturnSceneObjectId(int32_t id)
    {
        if (id != 0)
        {
            _recycleQueueSceneObjectId.push(id);
        }
    }
}

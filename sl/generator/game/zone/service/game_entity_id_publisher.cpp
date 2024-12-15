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

        if (_nextValue < 1000 || _recycleQueue.empty())
        {
            const game_entity_id_type result = game_entity_id_type(++_nextValue, 1);

            assert(result != game_entity_id_type(NULL_ID));

            return result;
        }

        assert(!_recycleQueue.empty());

        game_entity_id_type result = _recycleQueue.front();
        result.SetRecycleSequence(result.GetRecycleSequence() + 1);

        _recycleQueue.pop();

        return result;
    }

    void GameEntityIdPublisher::Return(GameEntityType type, game_entity_id_type id)
    {
        (void)type;

        _recycleQueue.push(id);
    }

    auto GameEntityIdPublisher::PublishSceneObjectId(GameEntityType type) -> int32_t
    {
        if (IsDynamicEntity(type))
        {
            if (_recycleIdQueueDynamicEntity.empty())
            {
                return ++_nextDynamicEntityId;
            }

            const int32_t result = _recycleIdQueueDynamicEntity.front();
            _recycleIdQueueDynamicEntity.pop();

            return result;
        }

        if (IsStaticEntity(type))
        {
            if (_recycleIdQueueStaticEntity.empty())
            {
                return ++_nextStaticEntityId;
            }

            const int32_t result = _recycleIdQueueStaticEntity.front();
            _recycleIdQueueStaticEntity.pop();

            return result;
        }


        return ++_nextUnknownEntityId;
    }

    void GameEntityIdPublisher::ReturnSceneObjectId(GameEntityType type, int32_t id)
    {
        if (IsDynamicEntity(type))
        {
            _recycleIdQueueDynamicEntity.push(id);

            return;
        }

        if (IsStaticEntity(type))
        {
            _recycleIdQueueStaticEntity.push(id);
        }
    }

    bool GameEntityIdPublisher::IsDynamicEntity(GameEntityType type)
    {
        switch (type)
        {
        case GameEntityType::Player:
        case GameEntityType::NPC:
        case GameEntityType::Enemy:
        case GameEntityType::EnemyChild:
        case GameEntityType::Summon:
        case GameEntityType::Pet:
            return true;
        }

        return false;
    }

    bool GameEntityIdPublisher::IsStaticEntity(GameEntityType type)
    {
        switch (type)
        {
        case GameEntityType::Prop:
        case GameEntityType::PropChild:
        case GameEntityType::Item:
        case GameEntityType::ItemChild:
        case GameEntityType::MiniPK:
            return true;
        }

        return false;
    }
}

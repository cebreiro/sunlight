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

    auto GameEntityIdPublisher::PublishSceneObjectId(GameEntityType type) -> int32_t
    {
        (void)type;
        // client 590CA0
        //if (type == GameEntityType::Unk)
        //{
        // range: 0x2FFFFFFF ~ 0x20000000
        // client: if ((id & 0x30000000) == 0x20000000) -> rotate on fixed position
        //}

        int32_t result = 0;

        if (_recycleQueueSceneObjectId.empty())
        {
            // if scene object id is 1100 (GameEntityType::Player),
            // a local player disappears when receives that object remove packet
            while (result == 0 || IsEntityType(result))
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

    void GameEntityIdPublisher::ReturnSceneObjectId(GameEntityType type, int32_t id)
    {
        (void)type;

        if (id != 0)
        {
            _recycleQueueSceneObjectId.push(id);
        }
    }

    bool GameEntityIdPublisher::IsEntityType(int32_t value)
    {
        switch (static_cast<GameEntityType>(value))
        {
        case GameEntityType::Player:
        case GameEntityType::NPC:
        case GameEntityType::Enemy:
        case GameEntityType::EnemyChild:
        case GameEntityType::FX:
        case GameEntityType::House:
        case GameEntityType::Prop:
        case GameEntityType::PropChild:
        case GameEntityType::Item:
        case GameEntityType::ItemChild:
        case GameEntityType::EventObj:
        case GameEntityType::Stage:
        case GameEntityType::StageTerrain:
        case GameEntityType::StageRoom:
        case GameEntityType::Widget:
        case GameEntityType::Unk1940:
        case GameEntityType::Unk2100:
        case GameEntityType::Unk2300:
        case GameEntityType::Unk2400:
        case GameEntityType::Unk3100:
        case GameEntityType::Unk3200:
            return true;
        case GameEntityType::None:;
        default:;
        }

        return false;
    }
}

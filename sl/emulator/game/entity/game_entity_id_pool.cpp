#include "game_entity_id_pool.h"

namespace sunlight
{
    auto GameEntityIdPool::Pop(GameEntityType type) -> game_entity_id_type
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

    auto GameEntityIdPool::Push(GameEntityType type, game_entity_id_type id)
    {
        (void)type;

        _recycleQueue.push(id);
    }
}

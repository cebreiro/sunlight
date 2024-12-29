#include "game_item_unique_id_publisher.h"

namespace sunlight
{
    GameItemUniqueIdPublisher::GameItemUniqueIdPublisher(int32_t zoneId, int32_t snowflakeValue)
        : _name(fmt::format("game_item_unique_id_publisher_{}", zoneId))
        , _snowflake(snowflakeValue)
    {
    }

    auto GameItemUniqueIdPublisher::GetName() const -> std::string_view
    {
        return _name;
    }

    auto GameItemUniqueIdPublisher::Publish() -> int64_t
    {
        return static_cast<int64_t>(_snowflake.Generate());
    }
}

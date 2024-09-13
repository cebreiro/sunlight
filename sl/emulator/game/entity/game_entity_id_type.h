#pragma once

namespace sunlight
{
    namespace detail
    {
        class GameEntityIdTypeTag;
    }

    using game_entity_id_type = ValueType<uint32_t, detail::GameEntityIdTypeTag>;
}

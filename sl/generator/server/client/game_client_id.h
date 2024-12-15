#pragma once

namespace sunlight
{
    namespace detail
    {
        class GameClientIdTag;
    }

    using game_client_id_type = ValueType<int64_t, detail::GameClientIdTag>;
}

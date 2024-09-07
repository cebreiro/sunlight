#pragma once
#include <variant>
#include "shared/execution/channel/channel.h"
#include "shared/network/buffer/buffer.h"
#include "shared/network/session/id.h"

namespace sunlight::session
{
    struct ReceiveEvent
    {
        id_type id = id_type::Default();
        Buffer buffer;
    };

    struct IoErrorEvent
    {
        id_type id = id_type::Default();
        boost::system::error_code errorCode;
    };

    struct DestructEvent
    {
        id_type id = id_type::Default();
    };

    using event_type = std::variant<ReceiveEvent, IoErrorEvent, DestructEvent>;
    using event_channel_type = Channel<event_type>;
}

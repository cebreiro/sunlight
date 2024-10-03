#pragma once

namespace sunlight
{
    // client 0x464E10
    ENUM_CLASS(TradeMessageType, int32_t,
        (Success, 1052)
        (Fail, 1051)
        (Cancel, 1601)
    )
}

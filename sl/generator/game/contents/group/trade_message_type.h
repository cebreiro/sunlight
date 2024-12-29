#pragma once

namespace sunlight
{
    ENUM_CLASS(TradeMessageType, int32_t,
        (Success, 1052)
        (Fail, 1051)
        (Cancel, 1601)
    )
}

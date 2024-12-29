#pragma once

namespace sunlight
{
    ENUM_CLASS(StreetVendorPurchaseResult, int32_t,
        (Success, 1000)
        (LockOfSpace, 1001)
        (LockOfMoney, 1002)
        (AlreadySold, 1003)
    )
}

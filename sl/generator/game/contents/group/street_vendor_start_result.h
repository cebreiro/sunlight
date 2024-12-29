#pragma once

namespace sunlight
{
    ENUM_CLASS(StreetVendorStartResult, int32_t,
        (Success, 1000)
        (NoSaleItemPrice, 1001)
        (NoSaleItem, 1002)
    )
}

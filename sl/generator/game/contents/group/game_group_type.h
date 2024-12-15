#pragma once

namespace sunlight
{
    ENUM_CLASS(GameGroupType, int32_t,
        (Null, -1)
        (Unk04, 4) // player billing
        (Unk05, 5) // player billing
        (Trade, 5001)
        (ItemMix, 5002)

        (StreetVendor, 6001)
        (Interior, 7000)
    )
}

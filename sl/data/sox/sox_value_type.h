#pragma once

namespace sunlight
{
    ENUM_CLASS(SoxValueType, int32_t,
        (None, 0)
        (String, 1)
        (Int, 2)
        (Float, 4)
    )

    inline auto ToSoxValueTypeFrom(int32_t value) -> SoxValueType
    {
        switch (value)
        {
        case 1:
            return SoxValueType::String;
        case 2:
        case 3:
        case 5:
            return SoxValueType::Int;
        case 4:
            return SoxValueType::Float;
        default:
            assert(false);
            return SoxValueType::None;
        }
    }
}

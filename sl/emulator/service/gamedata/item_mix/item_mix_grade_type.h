#pragma once

namespace sunlight
{
    ENUM_CLASS(ItemMixGradeType, int32_t,
        (Low, 0)
        (Middle, 1)
        (High, 2)
        (Super, 3)
    )

    constexpr int32_t item_mix_grade_count = static_cast<int32_t>(ItemMixGradeTypeMeta::GetSize());
}

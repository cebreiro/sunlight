#pragma once
#include "item_mix_grade_type.h"

namespace sunlight::sox
{
    class ItemmixJudgeTableTable;
}

namespace sunlight
{
    ENUM_CLASS(ItemMixGradeWeightType, int32_t,
        (Fail, 0)
        (Low, 1)
        (Middle, 2)
        (High, 3)
        (Super, 4)
    )

    constexpr int32_t item_mix_grade_level_size = 41;
    constexpr int32_t item_mix_grade_weight_size = static_cast<int32_t>(ItemMixGradeWeightTypeMeta::GetSize());

    class ItemMixGradeWeightData
    {
    public:
        static constexpr int32_t max_grade_level = 41;

    public:
        explicit ItemMixGradeWeightData(const sox::ItemmixJudgeTableTable& table);

        auto Find(int32_t gradeType, int32_t level) const -> const std::array<int32_t, item_mix_grade_weight_size>*;

    private:
        std::unordered_map<int32_t,
            std::array<std::array<int32_t, item_mix_grade_weight_size>, max_grade_level>> _weights;
    };
}

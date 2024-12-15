#include "item_mix_grade_weight_data.h"

#include "sl/generator/game/data/sox/itemmix_judge_table.h"

namespace sunlight
{
    ItemMixGradeWeightData::ItemMixGradeWeightData(const sox::ItemmixJudgeTableTable& table)
    {
        const std::vector<sox::ItemmixJudgeTable>& datas = table.Get();
        if (std::ssize(datas) != max_grade_level * 4)
        {
            throw std::runtime_error("invalid sox::ItemmixJudgeTable size");
        }

        auto iter = datas.begin();

        for (int32_t i = 1; i <= 4; ++i)
        {
            auto& weight = _weights[i];

            for (int32_t j = 0; j < max_grade_level; ++j)
            {
                const sox::ItemmixJudgeTable& data = *iter;

                weight[j] = { data.fail, data.gradeLow, data.gradeModdle, data.gradeHigh, data.gradSUper };

                ++iter;
            }
        }
    }

    auto ItemMixGradeWeightData::Find(int32_t gradeType, int32_t level) const -> const std::array<int32_t, item_mix_grade_weight_size>*
    {
        if (level <= 0 || level - 1 >= max_grade_level)
        {
            return nullptr;
        }

        const auto iter = _weights.find(gradeType);
        if (iter == _weights.end())
        {
            return nullptr;
        }

        return &iter->second[level - 1];
    }
}

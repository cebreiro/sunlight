#include "item_mix_skill_exp_modify_data.h"

namespace sunlight
{
    ItemMixSkillExpModifyData::ItemMixSkillExpModifyData(int64_t lastLevel)
        : _value(lastLevel, 0)
    {
    }

    auto ItemMixSkillExpModifyData::GetValue(int32_t level) const -> std::optional<int32_t>
    {
        const int32_t index = level - 1;
        if (index < 0 || index >= std::ssize(_value))
        {
            return std::nullopt;
        }

        return _value[index];
    }

    void ItemMixSkillExpModifyData::SetValue(int32_t level, int32_t value)
    {
        const int32_t index = level - 1;
        assert(index >= 0 && index < std::ssize(_value));

        _value[index] = value;
    }
}

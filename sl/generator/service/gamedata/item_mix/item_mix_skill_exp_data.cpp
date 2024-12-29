#include "item_mix_skill_exp_data.h"

namespace sunlight
{
    ItemMixSkillExpData::ItemMixSkillExpData(int32_t lastLLevel)
        : _value(lastLLevel, 0)
    {
    }

    auto ItemMixSkillExpData::GetExp(int32_t level) const -> std::optional<int32_t>
    {
        const int32_t index = level - 1;
        if (index < 0 || index >= std::ssize(_value))
        {
            return std::nullopt;
        }

        return _value[index];
    }

    void ItemMixSkillExpData::SetExp(int32_t level, int32_t value)
    {
        const int32_t index = level - 1;
        assert(index >= 0 && index < std::ssize(_value));

        _value[index] = value;
    }
}

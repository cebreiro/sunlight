#include "item_mix_difficulty_data.h"

namespace sunlight
{
    ItemMixDifficultyData::ItemMixDifficultyData(int64_t size)
        : _values(size, 0)
    {
    }

    auto ItemMixDifficultyData::GetDifficulty(int64_t level) const -> std::optional<int32_t>
    {
        const int64_t index = level - 1;
        if (index < 0 || index >= std::ssize(_values))
        {
            return std::nullopt;
        }

        return _values[index];
    }

    void ItemMixDifficultyData::SetDifficulty(int64_t level, int32_t value)
    {
        const int64_t index = level - 1;
        assert(index >= 0 && index < std::ssize(_values));

        _values[index] = value;
    }
}

#pragma once

namespace sunlight
{
    class ItemMixDifficultyData
    {
    public:
        ItemMixDifficultyData() = default;
        explicit ItemMixDifficultyData(int64_t size);

        auto GetDifficulty(int64_t level) const -> std::optional<int32_t>;

        void SetDifficulty(int64_t level, int32_t value);

    private:
        std::vector<int32_t> _values;
    };
}

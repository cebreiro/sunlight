#pragma once

namespace sunlight
{
    class ItemMixSkillExpData
    {
    public:
        ItemMixSkillExpData() = default;
        explicit ItemMixSkillExpData(int32_t lastLLevel);

        auto GetExp(int32_t level) const -> std::optional<int32_t>;

        void SetExp(int32_t level, int32_t value);

    private:
        std::vector<int32_t> _value;
    };
}

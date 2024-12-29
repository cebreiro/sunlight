#pragma once

namespace sunlight
{
    class ItemMixSkillExpModifyData
    {
    public:
        ItemMixSkillExpModifyData() = delete;
        explicit ItemMixSkillExpModifyData(int64_t lastLevel);

        auto GetValue(int32_t level) const -> std::optional<int32_t>;

        void SetValue(int32_t level, int32_t value);

    private:
        std::vector<int32_t> _value;
    };
}

#pragma once
#include "sl/generator/service/gamedata/item_mix/item_mix_group_data.h"

namespace sunlight::sox
{
    struct ItemmixSkill;
}

namespace sunlight
{
    class ItemMixData
    {
    public:
        explicit ItemMixData(const sox::ItemmixSkill& soxData);

        bool Add(const ItemMixGroupData& data);

        auto Find(int32_t skillLevel, int32_t itemId) const -> const ItemMixGroupMemberData*;

        auto GetDifficultyType() const -> int32_t;
        auto GetRequiredJobId() const -> int32_t;
        auto GetRequiredJobLevel() const -> int32_t;
        auto GetRequiredSkillId() const -> int32_t;
        auto GetRequiredSkillLevel() const -> int32_t;

    private:
        int32_t _skillId = 0;
        int32_t _difficultyType = 0;
        int32_t _requiredJobId = 0;
        int32_t _requiredJobLevel = 0;
        int32_t _requiredSkillId = 0;
        int32_t _requiredSkillLevel = 0;

        std::map<int32_t, ItemMixGroupData> _groups;
    };
}

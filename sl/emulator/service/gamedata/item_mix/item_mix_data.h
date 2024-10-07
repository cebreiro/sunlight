#pragma once
#include "sl/emulator/service/gamedata/item_mix/item_mix_group_data.h"

namespace sunlight
{
    class ItemMixData
    {
    public:
        explicit ItemMixData(int32_t skillId);

        bool Add(const ItemMixGroupData& data);

        auto Find(int32_t skillLevel, int32_t itemId) const -> const ItemMixGroupMemberData*;

    private:
        int32_t _skillId = 0;
        std::map<int32_t, ItemMixGroupData> _groups;
    };
}

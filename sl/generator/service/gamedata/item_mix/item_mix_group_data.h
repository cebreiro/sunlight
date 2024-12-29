#pragma once
#include "sl/generator/service/gamedata/item_mix/item_mix_group_member_data.h"

namespace sunlight
{
    class ItemMixGroupData
    {
    public:
        ItemMixGroupData(int32_t groupId, int32_t mixSkillId, int32_t mixSkillLevel);

        bool Add(const ItemMixGroupMemberData& data);

        auto FindMember(int32_t resultItemId) const -> const ItemMixGroupMemberData*;

        auto GetGroupId() const -> int32_t;
        auto GetMixSkillId() const -> int32_t;
        auto GetMixSkillLevel() const -> int32_t;

    private:
        int32_t _groupId = 0;
        int32_t _mixSkillId = 0;
        int32_t _mixSkillLevel = 0;

        std::unordered_map<int32_t, ItemMixGroupMemberData> _members;
    };
}

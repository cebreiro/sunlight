#include "item_mix_data.h"

namespace sunlight
{
    ItemMixData::ItemMixData(int32_t skillId)
        : _skillId(skillId)
    {
    }

    bool ItemMixData::Add(const ItemMixGroupData& data)
    {
        return _groups.try_emplace(data.GetMixSkillLevel(), data).second;
    }

    auto ItemMixData::Find(int32_t skillLevel, int32_t itemId) const -> const ItemMixGroupMemberData*
    {
        auto begin = _groups.begin();
        auto end = _groups.upper_bound(skillLevel);

        for (auto iter = begin; iter != end; ++iter)
        {
            if (const ItemMixGroupMemberData* data = iter->second.FindMember(itemId);
                data)
            {
                return data;
            }
        }

        return nullptr;
    }
}

#include "item_mix_group_data.h"

namespace sunlight
{
    ItemMixGroupData::ItemMixGroupData(int32_t groupId, int32_t mixSkillId, int32_t mixSkillLevel)
        : _groupId(groupId)
        , _mixSkillId(mixSkillId)
        , _mixSkillLevel(mixSkillLevel)
    {
    }

    bool ItemMixGroupData::Add(const ItemMixGroupMemberData& data)
    {
        return _members.try_emplace(data.GetResultItemId(), data).second;
    }

    auto ItemMixGroupData::FindMember(int32_t resultItemId) const -> const ItemMixGroupMemberData*
    {
        const auto iter = _members.find(resultItemId);

        return iter != _members.end() ? &iter->second : nullptr;
    }

    auto ItemMixGroupData::GetGroupId() const -> int32_t
    {
        return _groupId;
    }

    auto ItemMixGroupData::GetMixSkillId() const -> int32_t
    {
        return _mixSkillId;
    }

    auto ItemMixGroupData::GetMixSkillLevel() const -> int32_t
    {
        return _mixSkillLevel;
    }
}

#include "item_mix_data.h"

#include "sl/generator/game/data/sox/itemmix_skill.h"

namespace sunlight
{
    ItemMixData::ItemMixData(const sox::ItemmixSkill& soxData)
        : _skillId(soxData.iD)
        , _difficultyType(soxData.difficultyType)
        , _requiredJobId(soxData.requiredJobID)
        , _requiredJobLevel(soxData.requiredJobLevel)
        , _requiredSkillId(soxData.requiredSkillID)
        , _requiredSkillLevel(soxData.requiredSkillLevel)
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

    auto ItemMixData::GetDifficultyType() const -> int32_t
    {
        return _difficultyType;
    }

    auto ItemMixData::GetRequiredJobId() const -> int32_t
    {
        return _requiredJobId;
    }

    auto ItemMixData::GetRequiredJobLevel() const -> int32_t
    {
        return _requiredJobLevel;
    }

    auto ItemMixData::GetRequiredSkillId() const -> int32_t
    {
        return _requiredSkillId;
    }

    auto ItemMixData::GetRequiredSkillLevel() const -> int32_t
    {
        return _requiredSkillLevel;
    }
}

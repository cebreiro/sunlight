#include "item_mix_data_provider.h"

#include "sl/emulator/game/data/sox_table_set.h"
#include "sl/emulator/game/data/sox/itemmix_correcttable.h"
#include "sl/emulator/game/data/sox/itemmix_judge_table.h"
#include "sl/emulator/game/data/sox/itemmix_skill.h"
#include "sl/emulator/game/data/sox/itemmix_skillexp_correcttable.h"
#include "sl/emulator/game/data/sox/itemmix_skillexp_table.h"
#include "sl/emulator/game/data/sox/itemmix_table.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    ItemMixDataProvider::ItemMixDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet, const ItemDataProvider& itemDataProvider)
        : _weightData(tableSet.Get<sox::ItemmixJudgeTableTable>())
    {
        const sox::ItemmixTableTable& soxMixTable = tableSet.Get<sox::ItemmixTableTable>();

        std::unordered_multimap<int32_t, PtrNotNull<const sox::ItemmixTable>> index;
        index.reserve(soxMixTable.Get().size());

        for (const sox::ItemmixTable& data : soxMixTable.Get())
        {
            index.emplace(data.groupID, &data);
        }

        for (const sox::ItemmixSkill& soxItemMixSkillData : tableSet.Get<sox::ItemmixSkillTable>().Get())
        {
            const int32_t skillId = soxItemMixSkillData.iD;

            ItemMixData& mixData = _mixData.try_emplace(skillId, skillId).first->second;

            const std::initializer_list<std::pair<int32_t, int32_t>> list{
                std::pair(soxItemMixSkillData.mixGroupID1, soxItemMixSkillData.mixGroupID1Level),
                std::pair(soxItemMixSkillData.mixGroupID2, soxItemMixSkillData.mixGroupID2Level),
                std::pair(soxItemMixSkillData.mixGroupID3, soxItemMixSkillData.mixGroupID3Level),
                std::pair(soxItemMixSkillData.mixGroupID4, soxItemMixSkillData.mixGroupID4Level),
                std::pair(soxItemMixSkillData.mixGroupID5, soxItemMixSkillData.mixGroupID5Level),
                std::pair(soxItemMixSkillData.mixGroupID6, soxItemMixSkillData.mixGroupID6Level),
            };

            constexpr auto filter = [](const auto& pair) -> bool
                {
                    return pair.first != 0 && pair.second != 0;
                };

            for (const auto [groupId, groupLevel] : list | std::views::filter(filter))
            {
                const auto [begin, end] = index.equal_range(groupId);
                if (begin == end)
                {
                    SUNLIGHT_LOG_WARN(serviceLocator,
                        fmt::format("[{}] fail to find item mix group data. skill: {}, group: {}",
                            GetName(), skillId, groupId));

                    continue;
                }

                ItemMixGroupData groupData(groupId, skillId, groupLevel);

                for (auto iter = begin; iter != end; ++iter)
                {
                    const sox::ItemmixTable& soxItemMixData = *iter->second;

                    ItemMixGroupMemberData memberData(soxItemMixData);

                    std::array<int32_t, item_mix_grade_count> resultItemIdList = {
                        memberData.GetResultItemId(),
                        memberData.GetResultItemId() + 10000000,
                        memberData.GetResultItemId() + 20000000,
                        memberData.GetResultItemId() + 30000000,
                    };
                    std::array<const ItemData*, item_mix_grade_count> resultItemDataList = {};

                    for (int32_t i = 0; i < item_mix_grade_count; ++i)
                    {
                        resultItemDataList[i] = itemDataProvider.Find(resultItemIdList[i]);
                    }

                    if (std::ranges::all_of(resultItemDataList, [](const ItemData* data) -> bool
                        {
                            return data == nullptr;
                        }))
                    {
                        SUNLIGHT_LOG_WARN(serviceLocator,
                            fmt::format("[{}] fail to add item mix group data. result items are null. skill: {}, group: {}, index: {}",
                                GetName(), skillId, groupId, soxItemMixData.index));

                        continue;
                    }

                    memberData.SetResultItemData(resultItemDataList);

                    if (!groupData.Add(memberData))
                    {
                        SUNLIGHT_LOG_WARN(serviceLocator,
                            fmt::format("[{}] fail to add item mix group data. skill: {}, group: {}, index: {}",
                                GetName(), skillId, groupId, soxItemMixData.index));
                    }
                }

                if (!mixData.Add(groupData))
                {
                    SUNLIGHT_LOG_WARN(serviceLocator,
                        fmt::format("[{}] fail to add item mix group data. skill: {}, group: {}, group level: {}",
                            GetName(), skillId, groupId, groupData.GetMixSkillLevel()));
                }
            }
        }
    }

    auto ItemMixDataProvider::GetName() const -> std::string_view
    {
        return"item_mix_data_provider";
    }

    auto ItemMixDataProvider::Find(int32_t skillId) const -> const ItemMixData*
    {
        const auto iter = _mixData.find(skillId);

        return iter != _mixData.end() ? &iter->second : nullptr;
    }

    auto ItemMixDataProvider::FindWeight(int32_t gradeType, int32_t level) const -> const std::array<int32_t, item_mix_grade_weight_size>*
    {
        return _weightData.Find(gradeType, level);
    }
}

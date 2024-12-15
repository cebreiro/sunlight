#include "item_mix_data_provider.h"

#include "sl/generator/game/data/sox_table_set.h"
#include "sl/generator/game/data/sox/itemmix_correcttable.h"
#include "sl/generator/game/data/sox/itemmix_judge_table.h"
#include "sl/generator/game/data/sox/itemmix_skill.h"
#include "sl/generator/game/data/sox/itemmix_skillexp_correcttable.h"
#include "sl/generator/game/data/sox/itemmix_skillexp_table.h"
#include "sl/generator/game/data/sox/itemmix_table.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"
#include "sl/generator/service/gamedata/item_mix/item_mix_difficulty_data.h"
#include "sl/generator/service/gamedata/item_mix/item_mix_skill_exp_data.h"
#include "sl/generator/service/gamedata/item_mix/item_mix_skill_exp_modify_data.h"

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

            ItemMixData& mixData = _mixData.try_emplace(skillId, soxItemMixSkillData).first->second;

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

                    ItemMixGroupMemberData memberData(soxItemMixData, groupLevel);

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

        const sox::ItemmixCorrecttableTable& difficultyData = tableSet.Get<sox::ItemmixCorrecttableTable>();
        const std::vector<sox::ItemmixCorrecttable>& soxDifficultyList = difficultyData.Get();

        for (int64_t i = 0; i < difficulty_count; ++i)
        {
            _difficultyData[i] = std::make_unique<ItemMixDifficultyData>(soxDifficultyList.back().index);
        }

        for (const sox::ItemmixCorrecttable& soxDifficulty : soxDifficultyList)
        {
            _difficultyData[0]->SetDifficulty(soxDifficulty.index, soxDifficulty.difficultyA);
            _difficultyData[1]->SetDifficulty(soxDifficulty.index, soxDifficulty.difficultyB);
            _difficultyData[2]->SetDifficulty(soxDifficulty.index, soxDifficulty.difficultyC);
            _difficultyData[3]->SetDifficulty(soxDifficulty.index, soxDifficulty.difficultyD);
            _difficultyData[4]->SetDifficulty(soxDifficulty.index, soxDifficulty.difficultyE);
            _difficultyData[5]->SetDifficulty(soxDifficulty.index, soxDifficulty.difficultyF);
        }

        const sox::ItemmixSkillexpTableTable& skillExpData = tableSet.Get<sox::ItemmixSkillexpTableTable>();
        const std::vector<sox::ItemmixSkillexpTable>& soxSkillExpList = skillExpData.Get();

        for (int64_t i = 0; i < skill_exp_count; ++i)
        {
            _skillExpData[i] = std::make_unique<ItemMixSkillExpData>(soxSkillExpList.back().index);
        }

        for (const sox::ItemmixSkillexpTable& soxSkillExp : soxSkillExpList)
        {
            _skillExpData[0]->SetExp(soxSkillExp.index, soxSkillExp.difficultyA);
            _skillExpData[1]->SetExp(soxSkillExp.index, soxSkillExp.difficultyB);
            _skillExpData[2]->SetExp(soxSkillExp.index, soxSkillExp.difficultyC);
        }

        const sox::ItemmixSkillexpCorrecttableTable& skillExpModifyTable = tableSet.Get<sox::ItemmixSkillexpCorrecttableTable>();
        const std::vector<sox::ItemmixSkillexpCorrecttable>& skillExpModifyDataList = skillExpModifyTable.Get();

        for (int64_t i = 0; i < difficulty_count; ++i)
        {
            _skillExpModifiers[i] = std::make_unique<ItemMixSkillExpModifyData>(skillExpModifyDataList.back().index);
        }

        for (const sox::ItemmixSkillexpCorrecttable& soxSkillExpModifier : skillExpModifyDataList)
        {
            _skillExpModifiers[0]->SetValue(soxSkillExpModifier.index, soxSkillExpModifier.difficultyA);
            _skillExpModifiers[1]->SetValue(soxSkillExpModifier.index, soxSkillExpModifier.difficultyB);
            _skillExpModifiers[2]->SetValue(soxSkillExpModifier.index, soxSkillExpModifier.difficultyC);
            _skillExpModifiers[3]->SetValue(soxSkillExpModifier.index, soxSkillExpModifier.difficultyD);
            _skillExpModifiers[4]->SetValue(soxSkillExpModifier.index, soxSkillExpModifier.difficultyE);
            _skillExpModifiers[5]->SetValue(soxSkillExpModifier.index, soxSkillExpModifier.difficultyF);
        }
    }

    ItemMixDataProvider::~ItemMixDataProvider()
    {
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

    auto ItemMixDataProvider::GetDifficulty(int32_t difficultyLevel, int32_t level) const -> std::optional<int32_t>
    {
        const int32_t index = difficultyLevel - 1;
        if (index < 0 || index >= std::ssize(_difficultyData))
        {
            return std::nullopt;
        }

        return _difficultyData[index]->GetDifficulty(level);
    }

    auto ItemMixDataProvider::GetLevelUpExp(int32_t expType, int32_t currentLevel) const -> std::optional<int32_t>
    {
        const int32_t index = expType - 1;
        if (index < 0 || index >= std::ssize(_skillExpData))
        {
            return std::nullopt;
        }

        return _skillExpData[index]->GetExp(currentLevel);
    }

    auto ItemMixDataProvider::GetExpModifier(int32_t difficultyLevel, int32_t skillLevel) const -> std::optional<int32_t>
    {
        const int32_t index = difficultyLevel - 1;
        if (index < 0 || index >= std::ssize(_skillExpModifiers))
        {
            return std::nullopt;
        }

        return _skillExpModifiers[index]->GetValue(skillLevel);
    }
}

#include "item_mix_group_member_data.h"

#include "sl/generator/game/data/sox/itemmix_table.h"

namespace sunlight
{
    ItemMixGroupMemberData::ItemMixGroupMemberData(const sox::ItemmixTable& data, int32_t groupLevel)
        : _groupId(data.groupID)
        , _groupLevel(groupLevel)
        , _name(data.name)
        , _gradeType(data.gradeTableType)
        , _mixTimeMilli(data.mixTime)
        , _difficulty(data.difficulty)
        , _resultItemId(data.resultID)
        , _resultItemCount(data.count)
        , _materials([&data]() -> std::vector<ItemMixMaterialData>
            {
                std::initializer_list<std::pair<int32_t, int32_t>> list{
                    std::pair(data.material1ID, data.material1NU),
                    std::pair(data.material2ID, data.material2NU),
                    std::pair(data.material3ID, data.material3NU),
                    std::pair(data.material4ID, data.material4NU),
                };

                constexpr auto filter = [](const auto& pair) -> bool
                    {
                        return pair.first != 0 && pair.second > 0;
                    };
                constexpr auto transform = [](const auto& pair) -> ItemMixMaterialData
                    {
                        return ItemMixMaterialData{
                            .itemId = pair.first,
                            .quantity = pair.second,
                        };
                    };

                return std::ranges::to<std::vector>(list | std::views::filter(filter) | std::views::transform(transform));
            }())
    {
    }

    auto ItemMixGroupMemberData::GetGroupId() const -> int32_t
    {
        return _groupId;
    }

    auto ItemMixGroupMemberData::GetGroupLevel() const -> int32_t
    {
        return _groupLevel;
    }

    auto ItemMixGroupMemberData::GetName() const -> std::string_view
    {
        return _name;
    }

    auto ItemMixGroupMemberData::GetGradeType() const -> int32_t
    {
        return _gradeType;
    }

    auto ItemMixGroupMemberData::GetMixTimeMilli() const -> int32_t
    {
        return _mixTimeMilli;
    }

    auto ItemMixGroupMemberData::GetDifficulty() const -> int32_t
    {
        return _difficulty;
    }

    auto ItemMixGroupMemberData::GetResultItemId() const -> int32_t
    {
        return _resultItemId;
    }

    auto ItemMixGroupMemberData::GetResultItemCount() const -> int32_t
    {
        return _resultItemCount;
    }

    auto ItemMixGroupMemberData::GetMaterials() const -> const std::vector<ItemMixMaterialData>&
    {
        return _materials;
    }

    auto ItemMixGroupMemberData::GetResultItemDataList() const -> const std::array<const ItemData*, item_mix_grade_count>&
    {
        return _resultItemDataList;
    }

    void ItemMixGroupMemberData::SetResultItemData(const std::array<const ItemData*, item_mix_grade_count>& data)
    {
        _resultItemDataList = data;
    }
}

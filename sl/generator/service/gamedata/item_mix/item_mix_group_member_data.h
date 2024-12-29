#pragma once
#include "sl/generator/service/gamedata/item_mix/item_mix_grade_type.h"

namespace sunlight::sox
{
    struct ItemmixTable;
}

namespace sunlight
{
    class ItemData;
}

namespace sunlight
{
    struct ItemMixMaterialData
    {
        int32_t itemId = 0;
        int32_t quantity = 0;
    };

    class ItemMixGroupMemberData
    {
    public:
        ItemMixGroupMemberData(const sox::ItemmixTable& data, int32_t groupLevel);

        auto GetGroupId() const -> int32_t;
        auto GetGroupLevel() const -> int32_t;
        auto GetName() const -> std::string_view;
        auto GetGradeType() const -> int32_t;
        auto GetMixTimeMilli() const -> int32_t;
        auto GetDifficulty() const -> int32_t;

        auto GetResultItemId() const -> int32_t;
        auto GetResultItemCount() const -> int32_t;
        auto GetMaterials() const -> const std::vector<ItemMixMaterialData>&;
        auto GetResultItemDataList() const -> const std::array<const ItemData*, item_mix_grade_count>&;

        void SetResultItemData(const std::array<const ItemData*, item_mix_grade_count>& data);

    private:
        int32_t _groupId = 0;
        int32_t _groupLevel = 0;
        std::string _name;

        int32_t _gradeType = 0;
        int32_t _mixTimeMilli = 0;
        int32_t _difficulty = 0;
        int32_t _resultItemId = 0;
        int32_t _resultItemCount = 0;

        std::vector<ItemMixMaterialData> _materials;
        std::array<const ItemData*, item_mix_grade_count> _resultItemDataList = {};
    };
}

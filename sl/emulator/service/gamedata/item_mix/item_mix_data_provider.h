#pragma once
#include "sl/emulator/service/gamedata/item_mix/item_mix_data.h"
#include "sl/emulator/service/gamedata/item_mix/item_mix_grade_weight_data.h"

namespace sunlight
{
    class SoxTableSet;
    class ItemMixData;

    class ItemDataProvider;
}

namespace sunlight
{
    class ItemMixDataProvider
    {
    public:
        ItemMixDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet, const ItemDataProvider& itemDataProvider);

        auto GetName() const -> std::string_view;

        auto Find(int32_t skillId) const -> const ItemMixData*;
        auto FindWeight(int32_t gradeType, int32_t level) const -> const std::array<int32_t, item_mix_grade_weight_size>*;

    private:
        std::unordered_map<int32_t, ItemMixData> _mixData;
        ItemMixGradeWeightData _weightData;
    };
}

#pragma once
#include "sl/generator/service/gamedata/item_mix/item_mix_data.h"
#include "sl/generator/service/gamedata/item_mix/item_mix_grade_weight_data.h"

namespace sunlight
{
    class SoxTableSet;
    class ItemMixData;
    class ItemMixDifficultyData;
    class ItemMixSkillExpData;
    class ItemMixSkillExpModifyData;

    class ItemDataProvider;
}

namespace sunlight
{
    class ItemMixDataProvider
    {
    public:
        ItemMixDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet, const ItemDataProvider& itemDataProvider);
        ~ItemMixDataProvider();

        auto GetName() const -> std::string_view;

        auto Find(int32_t skillId) const -> const ItemMixData*;
        auto FindWeight(int32_t gradeType, int32_t level) const -> const std::array<int32_t, item_mix_grade_weight_size>*;

        auto GetDifficulty(int32_t difficultyLevel, int32_t level) const -> std::optional<int32_t>;
        auto GetLevelUpExp(int32_t expType, int32_t currentLevel) const -> std::optional<int32_t>;
        auto GetExpModifier(int32_t difficultyLevel, int32_t skillLevel)const -> std::optional<int32_t>;

    private:
        std::unordered_map<int32_t, ItemMixData> _mixData;
        ItemMixGradeWeightData _weightData;

        static constexpr int32_t difficulty_count = 6;
        std::array<UniquePtrNotNull<ItemMixDifficultyData>, difficulty_count> _difficultyData;

        static constexpr int32_t skill_exp_count = 3;
        std::array<UniquePtrNotNull<ItemMixSkillExpData>, skill_exp_count> _skillExpData;

        std::array<UniquePtrNotNull<ItemMixSkillExpModifyData>, difficulty_count> _skillExpModifiers;
    };
}

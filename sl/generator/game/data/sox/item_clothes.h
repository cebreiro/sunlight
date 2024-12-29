#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ItemClothes
    {
        explicit ItemClothes(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string name = {};
        int32_t generate = {};
        std::string explanation = {};
        int32_t invenWidth = {};
        int32_t invenHeight = {};
        int32_t maxOverlapCount = {};
        int32_t rarity = {};
        int32_t price = {};
        int32_t modelID = {};
        int32_t modelColor = {};
        int32_t dropSound = {};
        int32_t useQuickSlot = {};
        int32_t oneMoreItem = {};
        int32_t equipPos = {};
        int32_t armorClass = {};
        int32_t defense = {};
        int32_t protection = {};
        int32_t equipEffect1 = {};
        int32_t equipEffectValue1 = {};
        int32_t equipEffect2 = {};
        int32_t equipEffectValue2 = {};
        int32_t equipEffect3 = {};
        int32_t equipEffectValue3 = {};
        int32_t equipEffect4 = {};
        int32_t equipEffectValue4 = {};
        int32_t constraint1 = {};
        int32_t constraint2 = {};
        int32_t constraint3 = {};
        int32_t constraint4 = {};
        int32_t constraint5 = {};
        int32_t constraint6 = {};
        int32_t constraint7 = {};
        int32_t constraint8 = {};
        int32_t constraintValue1 = {};
        int32_t constraintValue2 = {};
        int32_t constraintValue3 = {};
        int32_t constraintValue4 = {};
        int32_t constraintValue5 = {};
        int32_t constraintValue6 = {};
        int32_t constraintValue7 = {};
        int32_t constraintValue8 = {};
        int32_t constraintOR1 = {};
        int32_t constraintOR2 = {};
        int32_t constraintOR3 = {};
        int32_t constraintOR4 = {};
        int32_t constraintOR5 = {};
        int32_t constraintOR6 = {};
        int32_t constraintOR7 = {};
        int32_t ableToSell = {};
        int32_t ableToTrade = {};
        int32_t ableToDrop = {};
        int32_t ableToDestroy = {};
        int32_t ableToStorage = {};
    };

    class ItemClothesTable final : public ISoxTable, public std::enable_shared_from_this<ItemClothesTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ItemClothes*;
        auto Get() const -> const std::vector<ItemClothes>&;

    private:
        std::unordered_map<int32_t, ItemClothes*> _umap;
        std::vector<ItemClothes> _vector;

    };
}
#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ItemEtc
    {
        explicit ItemEtc(StreamReader<std::vector<char>::const_iterator>& reader);

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
        int32_t money = {};
        int32_t bulletType = {};
        int32_t ableToSell = {};
        int32_t ableToTrade = {};
        int32_t ableToDrop = {};
        int32_t ableToDestroy = {};
        int32_t ableToStorage = {};
        int32_t skillID = {};
        int32_t grade = {};
        int32_t isTool = {};
        int32_t mixDistance = {};
        int32_t interfaceAniID = {};
        int32_t toolMotionID = {};
        int32_t charMotionLoopID = {};
        int32_t charMotionStartID = {};
        int32_t toolSoundID = {};
        int32_t toolEventSoundID = {};
    };

    class ItemEtcTable final : public ISoxTable, public std::enable_shared_from_this<ItemEtcTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ItemEtc*;
        auto Get() const -> const std::vector<ItemEtc>&;

    private:
        std::unordered_map<int32_t, ItemEtc*> _umap;
        std::vector<ItemEtc> _vector;

    };
}

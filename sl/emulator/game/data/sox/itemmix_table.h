#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ItemmixTable
    {
        explicit ItemmixTable(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t groupID = {};
        std::string name = {};
        int32_t gradeTableType = {};
        int32_t mixTime = {};
        int32_t difficulty = {};
        int32_t resultID = {};
        int32_t resultMax = {};
        int32_t count = {};
        int32_t material1ID = {};
        std::string material1Name = {};
        int32_t material1NU = {};
        int32_t material2ID = {};
        std::string material2Name = {};
        int32_t material2NU = {};
        int32_t material3ID = {};
        std::string material3Name = {};
        int32_t material3NU = {};
        int32_t material4ID = {};
        std::string material4Name = {};
        int32_t material4NU = {};
        int32_t maxDepth = {};
        int32_t depthID1 = {};
        int32_t depthID2 = {};
        int32_t depthID3 = {};
        int32_t depthID4 = {};
        int32_t expCorrect = {};
    };

    class ItemmixTableTable final : public ISoxTable, public std::enable_shared_from_this<ItemmixTableTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ItemmixTable*;
        auto Get() const -> const std::vector<ItemmixTable>&;

    private:
        std::unordered_map<int32_t, ItemmixTable*> _umap;
        std::vector<ItemmixTable> _vector;

    };
}

#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ItemmixJudgeTable
    {
        explicit ItemmixJudgeTable(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t fail = {};
        int32_t gradeLow = {};
        int32_t gradeModdle = {};
        int32_t gradeHigh = {};
        int32_t gradSUper = {};
    };

    class ItemmixJudgeTableTable final : public ISoxTable, public std::enable_shared_from_this<ItemmixJudgeTableTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ItemmixJudgeTable*;
        auto Get() const -> const std::vector<ItemmixJudgeTable>&;

    private:
        std::unordered_map<int32_t, ItemmixJudgeTable*> _umap;
        std::vector<ItemmixJudgeTable> _vector;

    };
}

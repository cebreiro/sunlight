#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ItemmixSkillexpCorrecttable
    {
        explicit ItemmixSkillexpCorrecttable(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t difficultyA = {};
        int32_t difficultyB = {};
        int32_t difficultyC = {};
        int32_t difficultyD = {};
        int32_t difficultyE = {};
        int32_t difficultyF = {};
    };

    class ItemmixSkillexpCorrecttableTable final : public ISoxTable, public std::enable_shared_from_this<ItemmixSkillexpCorrecttableTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ItemmixSkillexpCorrecttable*;
        auto Get() const -> const std::vector<ItemmixSkillexpCorrecttable>&;

    private:
        std::unordered_map<int32_t, ItemmixSkillexpCorrecttable*> _umap;
        std::vector<ItemmixSkillexpCorrecttable> _vector;

    };
}

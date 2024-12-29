#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct LvupDb
    {
        explicit LvupDb(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t lvupVg = {};
        int32_t lvupG = {};
        int32_t lvupL = {};
        int32_t lvupP = {};
    };

    class LvupDbTable final : public ISoxTable, public std::enable_shared_from_this<LvupDbTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const LvupDb*;
        auto Get() const -> const std::vector<LvupDb>&;

    private:
        std::unordered_map<int32_t, LvupDb*> _umap;
        std::vector<LvupDb> _vector;

    };
}

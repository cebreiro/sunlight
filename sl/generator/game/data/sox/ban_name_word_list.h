#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct BanNameWordList
    {
        explicit BanNameWordList(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string banStr = {};
        int32_t banType = {};
    };

    class BanNameWordListTable final : public ISoxTable, public std::enable_shared_from_this<BanNameWordListTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const BanNameWordList*;
        auto Get() const -> const std::vector<BanNameWordList>&;

    private:
        std::unordered_map<int32_t, BanNameWordList*> _umap;
        std::vector<BanNameWordList> _vector;

    };
}
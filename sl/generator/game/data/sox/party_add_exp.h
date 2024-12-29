#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct PartyAddExp
    {
        explicit PartyAddExp(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        float addExpFactor = {};
    };

    class PartyAddExpTable final : public ISoxTable, public std::enable_shared_from_this<PartyAddExpTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const PartyAddExp*;
        auto Get() const -> const std::vector<PartyAddExp>&;

    private:
        std::unordered_map<int32_t, PartyAddExp*> _umap;
        std::vector<PartyAddExp> _vector;

    };
}

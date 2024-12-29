#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct PropSet
    {
        explicit PropSet(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t numSubProp = {};
        int32_t subpropID = {};
    };

    class PropSetTable final : public ISoxTable, public std::enable_shared_from_this<PropSetTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const PropSet*;
        auto Get() const -> const std::vector<PropSet>&;

    private:
        std::unordered_map<int32_t, PropSet*> _umap;
        std::vector<PropSet> _vector;

    };
}
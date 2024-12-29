#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct ComboProbability
    {
        explicit ComboProbability(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        float enableCombo2 = {};
        float enableCombo3 = {};
        float enableCombo4 = {};
        float enableCombo5 = {};
        float enableCombo6 = {};
        float enableCombo7 = {};
        float enableCombo8 = {};
        float enableCombo9 = {};
        float enableCombo10 = {};
    };

    class ComboProbabilityTable final : public ISoxTable, public std::enable_shared_from_this<ComboProbabilityTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const ComboProbability*;
        auto Get() const -> const std::vector<ComboProbability>&;

    private:
        std::unordered_map<int32_t, ComboProbability*> _umap;
        std::vector<ComboProbability> _vector;

    };
}
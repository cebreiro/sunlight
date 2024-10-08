#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct LevelupAbility
    {
        explicit LevelupAbility(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t w1Ability = {};
        int32_t w1Quest = {};
        std::string w1Title = {};
        int32_t w2Ability = {};
        int32_t w2Quest = {};
        std::string w2Title = {};
        int32_t w3Ability = {};
        int32_t w3Quest = {};
        std::string w3Title = {};
        int32_t c1Ability = {};
        int32_t c1Quest = {};
        std::string c1Title = {};
        int32_t c2Ability = {};
        int32_t c2Quest = {};
        std::string c2Title = {};
        int32_t c3Ability = {};
        int32_t c3Quest = {};
        std::string c3Title = {};
        int32_t d1Ability = {};
        int32_t d1Quest = {};
        std::string d1Title = {};
        int32_t d2Ability = {};
        int32_t d2Quest = {};
        std::string d2Title = {};
        int32_t d3Ability = {};
        int32_t d3Quest = {};
        std::string d3Title = {};
        int32_t e1Ability = {};
        int32_t e1Quest = {};
        std::string e1Title = {};
        int32_t e2Ability = {};
        int32_t e2Quest = {};
        std::string e2Title = {};
        int32_t e3Ability = {};
        int32_t e3Quest = {};
        std::string e3Title = {};
        int32_t a1Ability = {};
        int32_t a1Quest = {};
        std::string a1Title = {};
        int32_t a2Ability = {};
        int32_t a2Quest = {};
        std::string a2Title = {};
        int32_t a3Ability = {};
        int32_t a3Quest = {};
        std::string a3Title = {};
    };

    class LevelupAbilityTable final : public ISoxTable, public std::enable_shared_from_this<LevelupAbilityTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const LevelupAbility*;
        auto Get() const -> const std::vector<LevelupAbility>&;

    private:
        std::unordered_map<int32_t, LevelupAbility*> _umap;
        std::vector<LevelupAbility> _vector;

    };
}

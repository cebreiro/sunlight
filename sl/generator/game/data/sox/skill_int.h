#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct SkillInt
    {
        explicit SkillInt(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string description = {};
        int32_t textureid = {};
        int32_t count = {};
    };

    class SkillIntTable final : public ISoxTable, public std::enable_shared_from_this<SkillIntTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const SkillInt*;
        auto Get() const -> const std::vector<SkillInt>&;

    private:
        std::unordered_map<int32_t, SkillInt*> _umap;
        std::vector<SkillInt> _vector;

    };
}
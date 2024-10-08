#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct Experience
    {
        explicit Experience(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t cLvExp = {};
        int32_t jLvExp = {};
    };

    class ExperienceTable final : public ISoxTable, public std::enable_shared_from_this<ExperienceTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const Experience*;
        auto Get() const -> const std::vector<Experience>&;

    private:
        std::unordered_map<int32_t, Experience*> _umap;
        std::vector<Experience> _vector;

    };
}

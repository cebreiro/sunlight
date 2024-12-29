#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct MonsterBase
    {
        explicit MonsterBase(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string name = {};
        int32_t level = {};
        int32_t exp = {};
        int32_t partyExp = {};
        int32_t race = {};
        int32_t hp = {};
        int32_t patkMin = {};
        int32_t patkMax = {};
        int32_t phrat = {};
        int32_t perat = {};
        int32_t pdefSlash = {};
        int32_t pdefBash = {};
        int32_t pdefPierce = {};
        int32_t matk = {};
        int32_t mres = {};
        int32_t elemF = {};
        int32_t elemW = {};
        int32_t elemL = {};
        int32_t speed = {};
        int32_t speedChase = {};
        int32_t debuffRes = {};
        int32_t lifetime = {};
        int32_t activationStartTime = {};
        int32_t activationEndTime = {};
        int32_t material = {};
    };

    class MonsterBaseTable final : public ISoxTable, public std::enable_shared_from_this<MonsterBaseTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const MonsterBase*;
        auto Get() const -> const std::vector<MonsterBase>&;

    private:
        std::unordered_map<int32_t, MonsterBase*> _umap;
        std::vector<MonsterBase> _vector;

    };
}

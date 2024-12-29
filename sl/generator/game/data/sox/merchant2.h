#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct Merchant2
    {
        explicit Merchant2(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string name = {};
        int32_t type = {};
        int32_t buyingFactor = {};
        int32_t sellingFactor = {};
        int32_t item01 = {};
        int32_t item02 = {};
        int32_t item03 = {};
        int32_t item04 = {};
        int32_t item05 = {};
        int32_t item06 = {};
        int32_t item07 = {};
        int32_t item08 = {};
        int32_t item09 = {};
        int32_t item10 = {};
        int32_t item11 = {};
        int32_t item12 = {};
        int32_t item13 = {};
        int32_t item14 = {};
        int32_t item15 = {};
        int32_t item16 = {};
        int32_t item17 = {};
        int32_t item18 = {};
        int32_t item19 = {};
        int32_t item20 = {};
        int32_t item21 = {};
        int32_t item22 = {};
        int32_t item23 = {};
        int32_t item24 = {};
        int32_t item25 = {};
        int32_t item26 = {};
        int32_t item27 = {};
        int32_t item28 = {};
        int32_t item29 = {};
        int32_t item30 = {};
        int32_t item31 = {};
        int32_t item32 = {};
        int32_t item33 = {};
        int32_t item34 = {};
        int32_t item35 = {};
        int32_t item36 = {};
        int32_t item37 = {};
        int32_t item38 = {};
        int32_t item39 = {};
        int32_t item40 = {};
        int32_t item41 = {};
        int32_t item42 = {};
        int32_t item43 = {};
        int32_t item44 = {};
        int32_t item45 = {};
        int32_t item46 = {};
        int32_t item47 = {};
        int32_t item48 = {};
        int32_t item49 = {};
        int32_t item50 = {};
        int32_t item51 = {};
        int32_t item52 = {};
        int32_t item53 = {};
        int32_t item54 = {};
        int32_t item55 = {};
        int32_t item56 = {};
        int32_t item57 = {};
        int32_t item58 = {};
        int32_t item59 = {};
        int32_t item60 = {};
        int32_t item61 = {};
        int32_t item62 = {};
        int32_t item63 = {};
        int32_t item64 = {};
        int32_t item65 = {};
        int32_t item66 = {};
        int32_t item67 = {};
        int32_t item68 = {};
        int32_t item69 = {};
        int32_t item70 = {};
        int32_t item71 = {};
        int32_t item72 = {};
        int32_t item73 = {};
        int32_t item74 = {};
        int32_t item75 = {};
        int32_t item76 = {};
        int32_t item77 = {};
        int32_t item78 = {};
        int32_t item79 = {};
        int32_t item80 = {};
    };

    class Merchant2Table final : public ISoxTable, public std::enable_shared_from_this<Merchant2Table>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const Merchant2*;
        auto Get() const -> const std::vector<Merchant2>&;

    private:
        std::unordered_map<int32_t, Merchant2*> _umap;
        std::vector<Merchant2> _vector;

    };
}

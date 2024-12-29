#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct AbilityPart2
    {
        explicit AbilityPart2(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t calc1Type = {};
        int32_t calc1Unit = {};
        float factor101 = {};
        float factor102 = {};
        float factor103 = {};
        float factor104 = {};
        float factor105 = {};
        float factor106 = {};
        float factor107 = {};
        float factor108 = {};
        float factor109 = {};
        float factor110 = {};
        float factor111 = {};
        float factor112 = {};
        float factor113 = {};
        float factor114 = {};
        float factor115 = {};
        float b11 = {};
        float b12 = {};
        float b13 = {};
        float b14 = {};
        float b15 = {};
        float b16 = {};
        float b17 = {};
        float b18 = {};
        float b19 = {};
        float b110 = {};
        float b111 = {};
        float b112 = {};
        float b113 = {};
        float b114 = {};
        float b115 = {};
        int32_t calc2Type = {};
        int32_t calc2Unit = {};
        float factor201 = {};
        float factor202 = {};
        float factor203 = {};
        float factor204 = {};
        float factor205 = {};
        float factor206 = {};
        float factor207 = {};
        float factor208 = {};
        float factor209 = {};
        float factor210 = {};
        float factor211 = {};
        float factor212 = {};
        float factor213 = {};
        float factor214 = {};
        float factor215 = {};
        float b21 = {};
        float b22 = {};
        float b23 = {};
        float b24 = {};
        float b25 = {};
        float b26 = {};
        float b27 = {};
        float b28 = {};
        float b29 = {};
        float b210 = {};
        float b211 = {};
        float b212 = {};
        float b213 = {};
        float b214 = {};
        float b215 = {};
        int32_t calc3Type = {};
        int32_t calc3Unit = {};
        float factor301 = {};
        float factor302 = {};
        float factor303 = {};
        float factor304 = {};
        float factor305 = {};
        float factor306 = {};
        float factor307 = {};
        float factor308 = {};
        float factor309 = {};
        float factor310 = {};
        float factor311 = {};
        float factor312 = {};
        float factor313 = {};
        float factor314 = {};
        float factor315 = {};
        float b31 = {};
        float b32 = {};
        float b33 = {};
        float b34 = {};
        float b35 = {};
        float b36 = {};
        float b37 = {};
        float b38 = {};
        float b39 = {};
        float b310 = {};
        float b311 = {};
        float b312 = {};
        float b313 = {};
        float b314 = {};
        float b315 = {};
    };

    class AbilityPart2Table final : public ISoxTable, public std::enable_shared_from_this<AbilityPart2Table>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const AbilityPart2*;
        auto Get() const -> const std::vector<AbilityPart2>&;

    private:
        std::unordered_map<int32_t, AbilityPart2*> _umap;
        std::vector<AbilityPart2> _vector;

    };
}

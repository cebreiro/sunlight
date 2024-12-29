#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct TileFx
    {
        explicit TileFx(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t enterolBellsyurot = {};
        int32_t enterolEnterance = {};
        int32_t enterolTolluen = {};
        int32_t enterolTrendol = {};
        int32_t enterolDesert = {};
        int32_t enterolMountain = {};
        int32_t enterolGrass = {};
        int32_t enterolWilderness = {};
        int32_t enterolMineField = {};
        int32_t enterolBeastDen = {};
        int32_t enterolAntCavern = {};
        int32_t enterolEnterolMaze = {};
        int32_t enterolUnderground = {};
        int32_t enterolLaboratory = {};
        int32_t enterolMestaruin = {};
        int32_t enterolTolluenruin = {};
    };

    class TileFxTable final : public ISoxTable, public std::enable_shared_from_this<TileFxTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const TileFx*;
        auto Get() const -> const std::vector<TileFx>&;

    private:
        std::unordered_map<int32_t, TileFx*> _umap;
        std::vector<TileFx> _vector;

    };
}

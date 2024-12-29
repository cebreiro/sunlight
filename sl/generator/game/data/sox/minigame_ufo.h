#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct MinigameUfo
    {
        explicit MinigameUfo(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string ufoname = {};
        int32_t cost = {};
        int32_t velocity = {};
        int32_t accel = {};
        int32_t radius = {};
        int32_t mingaugespeed = {};
        int32_t maxgaugespeed = {};
        int32_t mingaugemove = {};
        int32_t maxgaugemove = {};
        int32_t maxRarity = {};
        int32_t respawnTime = {};
        int32_t item1 = {};
        int32_t item2 = {};
        int32_t item3 = {};
        int32_t item4 = {};
        int32_t item5 = {};
        int32_t item6 = {};
    };

    class MinigameUfoTable final : public ISoxTable, public std::enable_shared_from_this<MinigameUfoTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const MinigameUfo*;
        auto Get() const -> const std::vector<MinigameUfo>&;

    private:
        std::unordered_map<int32_t, MinigameUfo*> _umap;
        std::vector<MinigameUfo> _vector;

    };
}
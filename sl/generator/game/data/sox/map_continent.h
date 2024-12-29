#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct MapContinent
    {
        explicit MapContinent(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t continentId = {};
        int32_t zoneId = {};
        int32_t zoneX1 = {};
        int32_t zoneY1 = {};
        int32_t zoneX2 = {};
        int32_t zoneY2 = {};
        int32_t zoneNameX1 = {};
        int32_t zoneNameY1 = {};
        int32_t link01X1 = {};
        int32_t link01Y1 = {};
        int32_t link02X1 = {};
        int32_t link02Y1 = {};
        int32_t link03X1 = {};
        int32_t link03Y1 = {};
        int32_t link04X1 = {};
        int32_t link04Y1 = {};
        int32_t link05X1 = {};
        int32_t link05Y1 = {};
        int32_t link06X1 = {};
        int32_t link06Y1 = {};
        int32_t dungeon01String = {};
        int32_t dungeon01X1 = {};
        int32_t dungeon01Y1 = {};
        int32_t dungeon02String = {};
        int32_t dungeon02X1 = {};
        int32_t dungeon02Y1 = {};
        int32_t dungeon03String = {};
        int32_t dungeon03X1 = {};
        int32_t dungeon03Y1 = {};
        int32_t dungeon04String = {};
        int32_t dungeon04X1 = {};
        int32_t dungeon04Y1 = {};
        int32_t dungeon05String = {};
        int32_t dungeon05X1 = {};
        int32_t dungeon05Y1 = {};
    };

    class MapContinentTable final : public ISoxTable, public std::enable_shared_from_this<MapContinentTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const MapContinent*;
        auto Get() const -> const std::vector<MapContinent>&;

    private:
        std::unordered_map<int32_t, MapContinent*> _umap;
        std::vector<MapContinent> _vector;

    };
}

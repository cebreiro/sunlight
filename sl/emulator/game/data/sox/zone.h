#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct Zone
    {
        explicit Zone(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        std::string zoneFilename = {};
        int32_t zoneType = {};
        std::string zoneDesc = {};
        int32_t zoneBgm = {};
        int32_t zoneWeatherDbid = {};
        int32_t zoneAvatarLight = {};
        int32_t zoneAmbient = {};
        int32_t zoneEntryLv = {};
        int32_t zoneUseability = {};
        int32_t zoneUsereturn = {};
    };

    class ZoneTable final : public ISoxTable, public std::enable_shared_from_this<ZoneTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const Zone*;
        auto Get() const -> const std::vector<Zone>&;

    private:
        std::unordered_map<int32_t, Zone*> _umap;
        std::vector<Zone> _vector;

    };
}

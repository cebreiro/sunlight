#pragma once
#include "sl/data/map/map_file.h"

namespace sunlight
{
    class MapDataProvider
    {
    public:
        explicit MapDataProvider(const std::filesystem::path& assetDirectory);
        ~MapDataProvider();

        auto FindMap(int32_t id) const -> const MapFile*;

    private:
        std::unordered_map<int32_t, MapFile> _mapFiles;
    };
}

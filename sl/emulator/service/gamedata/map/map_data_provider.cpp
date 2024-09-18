#include "map_data_provider.h"

#include <boost/lexical_cast.hpp>
#include "sl/data/map/map_stage.h"

namespace sunlight
{
    MapDataProvider::MapDataProvider(const std::filesystem::path& assetDirectory)
    {
        for (const auto& entry : std::filesystem::directory_iterator(assetDirectory))
        {
            const std::filesystem::path& path = entry.path();

            if (!path.has_extension())
            {
                continue;
            }

            if (::_stricmp(path.extension().string().c_str(), ".map") == 0)
            {
                const int32_t mapId = boost::lexical_cast<int32_t>(path.stem().string());

                [[maybe_unused]]
                const bool inserted = _mapFiles.try_emplace(mapId, MapFile::CreateFrom(path)).second;
                assert(inserted);
            }
        }
    }

    MapDataProvider::~MapDataProvider()
    {
    }

    auto MapDataProvider::FindMap(int32_t id) const -> const MapFile*
    {
        const auto iter = _mapFiles.find(id);

        return iter != _mapFiles.end() ? &iter->second : nullptr;
    }
}

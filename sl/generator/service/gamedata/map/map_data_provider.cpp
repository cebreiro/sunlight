#include "map_data_provider.h"

#include <boost/lexical_cast.hpp>
#include "sl/data/map/map_stage.h"
#include "sl/data/map/map_stage_terrain.h"

namespace sunlight
{
    MapDataProvider::MapDataProvider(const ServiceLocator& serviceLocator, const std::filesystem::path& assetDirectory)
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
            else if (::_stricmp(path.extension().string().c_str(), ".nes") == 0)
            {
                const int32_t mapId = boost::lexical_cast<int32_t>(path.stem().string());

                [[maybe_unused]]
                const bool inserted = _nesFiles.try_emplace(mapId, NesFile::CreateFrom(path)).second;
                assert(inserted);
            }
        }

        for (const auto& [mapId, mapFile] : _mapFiles)
        {
            for (const MapStage& mapStage : mapFile.stages | notnull::reference)
            {
                if (!mapStage.terrain)
                {
                    continue;
                }

                for (const MapEventObjectV5& eventV5 : mapStage.terrain->eventsV5)
                {
                    switch (eventV5.style)
                    {
                    case 2001:
                    case 5001:
                    {
                        const int32_t linkId = eventV5.reserved1;
                        const auto& [center, yaw] = ExtractPositionAndYaw(eventV5.transform);

                        if (!_mapGateData.try_emplace(std::make_pair(mapId, linkId), MapGateData{
                            .mapId = mapId,
                            .linkId = linkId,
                            .centerPosition = Eigen::Vector2f(center.x(), center.y()),
                            .yaw = yaw,
                        }).second)
                        {
                            SUNLIGHT_LOG_CRITICAL(serviceLocator,
                                fmt::format("[{}] fail to insert map gate data. duplicated id. map: {}, ld: {}",
                                    GetName(), mapId, linkId));
                        }
                    }
                    break;
                    }
                }
            }
        }
    }

    MapDataProvider::~MapDataProvider()
    {
    }

    auto MapDataProvider::GetName() const -> std::string_view
    {
        return "map_data_provider";
    }

    auto MapDataProvider::FindMap(int32_t id) const -> const MapFile*
    {
        const auto iter = _mapFiles.find(id);

        return iter != _mapFiles.end() ? &iter->second : nullptr;
    }

    auto MapDataProvider::FindMapGateData(int32_t zoneId, int32_t linkId) const -> const MapGateData*
    {
        const auto iter = _mapGateData.find(std::make_pair(zoneId, linkId));

        return iter != _mapGateData.end() ? &iter->second : nullptr;
    }

    auto MapDataProvider::FindNesFile(int32_t zoneId) const -> const NesFile*
    {
        const auto iter = _nesFiles.find(zoneId);

        return iter != _nesFiles.end() ? &iter->second : nullptr;
    }

    auto MapDataProvider::FindNesScript(int32_t zoneId, int32_t scriptId) const -> const NesScript*
    {
        const auto iter = _nesFiles.find(zoneId);
        if (iter == _nesFiles.end())
        {
            return nullptr;
        }

        const auto& index = iter->second.scriptIdIndex;
        const auto iter2 = index.find(scriptId);

        return iter2 != index.end() ? iter2->second : nullptr;
    }

    auto MapDataProvider::FindNesScriptCall(int32_t zoneId, int32_t entityId) const -> const NesScriptCall*
    {
        const auto iter = _nesFiles.find(zoneId);
        if (iter == _nesFiles.end())
        {
            return nullptr;
        }

        const auto& index = iter->second.callerIdIndex;
        const auto iter2 = index.find(entityId);

        return iter2 != index.end() ? iter2->second : nullptr;
    }

    auto MapDataProvider::ExtractPositionAndYaw(const Eigen::Matrix4f& matrix) -> std::pair<Eigen::Vector3f, float>
    {
        std::pair<Eigen::Vector3f, float> result;
        result.first = matrix.block<1, 3>(3, 0).transpose();
        result.second = std::atan2f(matrix(1, 0), matrix(0, 0)) * 180.0f / static_cast<float>(std::numbers::pi);

        return result;
    }
}

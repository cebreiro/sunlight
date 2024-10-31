#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "sl/data/map/map_file.h"
#include "sl/emulator/service/gamedata/map/map_gate_data.h"

namespace sunlight
{
    class MapDataProvider
    {
    public:
        MapDataProvider(const ServiceLocator& serviceLocator, const std::filesystem::path& assetDirectory);
        ~MapDataProvider();

        auto GetName() const -> std::string_view;

    public:
        auto FindMap(int32_t id) const -> const MapFile*;
        auto FindMapGateData(int32_t zoneId, int32_t linkId) const -> const MapGateData*;

    public:
        static auto ExtractPositionAndYaw(const Eigen::Matrix4f& matrix) -> std::pair<Eigen::Vector3f, float>;

    private:
        std::unordered_map<int32_t, MapFile> _mapFiles;
        boost::unordered::unordered_flat_map<std::pair<int32_t, int32_t>, MapGateData> _mapGateData;
    };
}

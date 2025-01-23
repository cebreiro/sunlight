#pragma once
#include "sl/generator/api/generated/dto.pb.h"

namespace sunlight
{
    class Zone;
    class ZoneServer;
}

namespace sunlight
{
    class World
    {
    public:
        World(const ServiceLocator& serviceLocator, int32_t id);
        ~World();

        bool Add(SharedPtrNotNull<ZoneServer> server, SharedPtrNotNull<Zone> zone);

        auto GetId() const -> int32_t;
        auto GetZoneInfo() const -> std::vector<api::ZoneInfo>;

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _id = 0;

        std::unordered_map<int32_t, std::string> _zoneNames;
        std::unordered_map<int32_t, std::pair<SharedPtrNotNull<ZoneServer>, SharedPtrNotNull<Zone>>> _zones;
    };
}

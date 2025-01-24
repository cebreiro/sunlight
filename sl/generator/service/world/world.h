#pragma once
#include "sl/generator/api/generated/dto.pb.h"

namespace sunlight
{
    class CommunityService;

    class Zone;
    class ZoneServer;
}

namespace sunlight
{
    class World
    {
    public:
        World(const ServiceLocator& serviceLocator, int32_t id, execution::IExecutor& executor);
        ~World();

        bool Add(SharedPtrNotNull<ZoneServer> server, SharedPtrNotNull<Zone> zone);

        auto GetId() const -> int32_t;
        auto GetServiceLocator() -> ServiceLocator&;
        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetZoneInfo() const -> std::vector<api::ZoneInfo>;

    private:
        ServiceLocator _serviceLocator;
        int32_t _id = 0;

        SharedPtrNotNull<CommunityService> _communityService;

        std::unordered_map<int32_t, std::string> _zoneNames;
        std::unordered_map<int32_t, std::pair<SharedPtrNotNull<ZoneServer>, SharedPtrNotNull<Zone>>> _zones;
    };
}

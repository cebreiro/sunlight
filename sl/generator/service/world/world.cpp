#include "world.h"

#include "sl/generator/game/zone/zone.h"
#include "sl/generator/server/zone_server.h"

namespace sunlight
{
    World::~World()
    {
    }

    bool World::Add(SharedPtrNotNull<ZoneServer> server, SharedPtrNotNull<Zone> zone)
    {
        assert(server->GetZoneId() == zone->GetId());

        const int32_t zoneId = server->GetZoneId();

        if (!_zones.try_emplace(zoneId, std::move(server), std::move(zone)).second)
        {
            return false;
        }

        return true;
    }

    auto World::GetId() const -> int32_t
    {
        return _id;
    }

    auto World::GetZoneInfo() const -> std::vector<api::ZoneInfo>
    {
        std::vector<api::ZoneInfo> result;
        result.reserve(_zones.size());

        for (const auto& [server, zone] : _zones | std::views::values)
        {
            api::ZoneInfo& zoneInfo = result.emplace_back();

            zoneInfo.set_id(zone->GetId());
            zoneInfo.set_port(server->GetListenPort());
        }

        return result;
    }
}

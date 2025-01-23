#include "world.h"

#include <boost/lexical_cast.hpp>
#include "sl/generator/game/data/sox/zone.h"
#include "sl/generator/game/zone/zone.h"
#include "sl/generator/server/zone_server.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"

namespace sunlight
{
    World::World(const ServiceLocator& serviceLocator, int32_t id)
        : _serviceLocator(serviceLocator)
        , _id(id)
    {
        const sox::ZoneTable& zoneTable = _serviceLocator.Get<GameDataProvideService>().Get<sox::ZoneTable>();

        for (const sox::Zone& zoneData : zoneTable.Get())
        {
            try
            {
                const int32_t zoneId = boost::lexical_cast<int32_t>(zoneData.zoneFilename);

                _zoneNames.try_emplace(zoneId, zoneData.zoneDesc);
            }
            catch (...)
            {
            }
        }
    }

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

            if (const auto iter = _zoneNames.find(zone->GetId()); iter != _zoneNames.end())
            {
                zoneInfo.set_name(iter->second);
            }
        }

        return result;
    }
}

#include "world_service.h"

#include <boost/scope/scope_exit.hpp>
#include "sl/generator/game/zone/zone.h"
#include "sl/generator/server/zone_server.h"
#include "sl/generator/service/community/community_service.h"
#include "sl/generator/service/world/world.h"

namespace sunlight
{
    WorldService::WorldService(const ServiceLocator& serviceLocator,
        execution::AsioExecutor& asioExecutor, execution::IExecutor& gameExecutor)
        : _serviceLocator(serviceLocator)
        , _asioExecutor(asioExecutor)
        , _gameExecutor(gameExecutor)
        , _strand(std::make_shared<Strand>(_gameExecutor.SharedFromThis()))
    {
    }

    WorldService::~WorldService()
    {
    }

    auto WorldService::Run() -> Future<void>
    {
        co_return;
    }

    void WorldService::Shutdown()
    {
    }

    void WorldService::Join(boost::system::error_code& ec)
    {
        (void)ec;
    }

    auto WorldService::GetName() const -> std::string_view
    {
        return "world_service";
    }

    auto WorldService::StartZone(int32_t worldId, int32_t zoneId, uint16_t port) -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        World& world = AddOrGetWorld(worldId);

        auto zone = std::make_shared<Zone>(world.GetServiceLocator(), _gameExecutor, worldId, zoneId);
        boost::scope::scope_exit exit([&zone]()
            {
                if (zone)
                {
                    zone->Shutdown();
                }
            });

        zone->Initialize();

        auto server = std::make_shared<ZoneServer>(world.GetServiceLocator(), _asioExecutor, zone);
        if (!server->StartUp(port))
        {
            throw std::runtime_error(fmt::format("[{}] fail to open port: {}. world_id: {}, zone_id: {}",
                GetName(), port, worldId, zoneId));
        }

        zone->Start();

        world.Add(std::move(server), std::move(zone));

        co_return;
    }

    auto WorldService::StopZone(int32_t worldId, int32_t zoneId) -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        World* world = FindWorld(worldId);
        if (!world)
        {
            throw std::runtime_error(fmt::format("[{}] fail to find world. world_id: {}",
                GetName(), worldId));
        }

        std::shared_ptr<ZoneServer> server;
        std::shared_ptr<Zone> zone;

        if (!world->Remove(zoneId, server, zone))
        {
            throw std::runtime_error(fmt::format("[{}] zone is not open. world_id: {}, zone_id: {}",
                GetName(), worldId, zoneId));
        }

        assert(server);
        assert(zone);

        server->Shutdown();
        zone->Shutdown();

        co_await zone->Join();

        co_return;
    }

    auto WorldService::GetWorldInfo() -> Future<std::vector<api::WorldInfo>>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        std::vector<api::WorldInfo> result;
        result.reserve(_worlds.size());

        for (const World& world : _worlds | std::views::values | notnull::reference)
        {
            api::WorldInfo& worldInfo = result.emplace_back();

            worldInfo.set_id(world.GetId());

            for (api::ZoneInfo& zoneInfo : world.GetZoneInfo())
            {
                worldInfo.mutable_open_zone_list()->Add(std::move(zoneInfo));
            }
        }

        // NRVO does not apply here. it is intended to select T&& overload in coroutine_traits
        co_return std::move(result);
    }

    auto WorldService::GetUserCount() -> Future<int32_t>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        int32_t result = 0;

        boost::container::small_vector<Future<int32_t>, 4> futures;

        for (const World& world : _worlds | std::views::values | notnull::reference)
        {
            futures.emplace_back(world.GetServiceLocator().Get<CommunityService>().GetUserCount());
        }

        co_await WaitAll(*_strand, futures);

        for (auto& future : futures)
        {
            result += future.Get();
        }

        co_return result;
    }

    auto WorldService::AddOrGetWorld(int32_t worldId) -> World&
    {
        auto iter = _worlds.find(worldId);
        if (iter == _worlds.end())
        {
            iter = _worlds.try_emplace(worldId, std::make_unique<World>(_serviceLocator, worldId, _gameExecutor)).first;
        }

        return *iter->second;
    }

    auto WorldService::FindWorld(int32_t worldId) -> World*
    {
        const auto iter = _worlds.find(worldId);

        return iter != _worlds.end() ? iter->second.get() : nullptr;
    }
}

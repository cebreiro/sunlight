#pragma once
#include "sl/generator/api/generated/dto.pb.h"
#include "sl/generator/service/emulation_service_interface.h"

namespace sunlight::execution
{
    class AsioExecutor;
}

namespace sunlight
{
    class World;

    class Zone;
    class ZoneServer;
}

namespace sunlight
{
    class WorldService final
        : public IEmulationService
        , public std::enable_shared_from_this<WorldService>
    {
    public:
        WorldService(const ServiceLocator& serviceLocator,
            execution::AsioExecutor& asioExecutor,
            execution::IExecutor& gameExecutor);
        ~WorldService();

        auto Run() -> Future<void> override;
        void Shutdown() override;
        void Join(boost::system::error_code& ec) override;

        auto GetName() const -> std::string_view override;

    public:
        auto StartZone(int32_t worldId, int32_t zoneId, uint16_t port) -> Future<void>;
        auto StopZone(int32_t worldId, int32_t zoneId) -> Future<void>;

        auto GetWorldInfo() -> Future<std::vector<api::WorldInfo>>;
        auto GetUserCount() -> Future<int32_t>;

    private:
        auto AddOrGetWorld(int32_t worldId) -> World&;

        auto FindWorld(int32_t worldId) -> World*;

    private:
        const ServiceLocator& _serviceLocator;
        execution::AsioExecutor& _asioExecutor;
        execution::IExecutor& _gameExecutor;

        SharedPtrNotNull<Strand> _strand;

        std::unordered_map<int32_t, UniquePtrNotNull<World>> _worlds;
    };
}

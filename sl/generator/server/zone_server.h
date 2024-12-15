#pragma once
#include "shared/network/server/server.h"
#include "sl/generator/server/server_type.h"

namespace sunlight
{
    class ServerConnection;
    class ZonePacketC2SHandler;
    class Zone;
}

namespace sunlight
{
    class ZoneServer final : public Server
    {
    public:
        static constexpr ServerType TYPE = ServerType::Zone;

    public:
        ZoneServer(execution::AsioExecutor& executor, execution::IExecutor& gameExecutor, int8_t worldId, int32_t zoneId);
        ~ZoneServer();

        void Initialize(ServiceLocator& serviceLocator) override;
        void StartUp(uint16_t listenPort) override;

    public:
        auto GetWorldId() const -> int8_t;
        auto GetZoneId() const -> int32_t;
        auto GetZone() -> Zone&;
        auto GetZone() const -> const Zone&;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<execution::IExecutor> _gameExecutor;
        int8_t _worldId = 0;
        int32_t _zoneId = 0;
        SharedPtrNotNull<Zone> _zone;

        SharedPtrNotNull<ZonePacketC2SHandler> _handler;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<ServerConnection>> _connections;
    };
}

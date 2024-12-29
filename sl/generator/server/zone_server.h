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
        ZoneServer(const ServiceLocator& serviceLocator, execution::AsioExecutor& executor, Zone& zone);
        ~ZoneServer();

    public:
        auto GetZoneId() const -> int32_t;
        auto GetZone() -> Zone&;
        auto GetZone() const -> const Zone&;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

    private:
        Zone& _zone;

        SharedPtrNotNull<ZonePacketC2SHandler> _handler;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<ServerConnection>> _connections;
    };
}

#pragma once
#include "shared/network/server/server.h"
#include "sl/emulator/server/server_type.h"

namespace sunlight
{
    class ServerConnection;
    class ZonePacketC2SHandler;
}

namespace sunlight
{
    class ZoneServer final : public Server
    {
    public:
        static constexpr ServerType TYPE = ServerType::Zone;

    public:
        ZoneServer(execution::AsioExecutor& executor, execution::IExecutor& gameExecutor, int32_t zoneId);

        void Initialize(ServiceLocator& serviceLocator) override;

    public:
        auto GetZoneId() const -> int32_t;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<execution::IExecutor> _gameExecutor;
        int32_t _zoneId = 0;

        SharedPtrNotNull<ZonePacketC2SHandler> _handler;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<ServerConnection>> _connections;
    };
}

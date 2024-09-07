#pragma once
#include "shared/network/server/server.h"
#include "sl/emulator/server/server_type.h"
#include "sl/emulator/server/client/game_client_id.h"

namespace sunlight
{
    class ServerConnection;
    class LoginPacketC2SHandler;
}

namespace sunlight
{
    class LoginServer final : public Server
    {
    public:
        static constexpr ServerType TYPE = ServerType::Login;

    public:
        explicit LoginServer(execution::AsioExecutor& executor);

        void Initialize(ServiceLocator& serviceLocator) override;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        auto PublishGameClientId() -> game_client_id_type;

        auto FindConnection(session::id_type id) -> ServerConnection*;

    private:
        ServiceLocator _serviceLocator;

        std::atomic<int64_t> _nextGameClientId = 0;

        SharedPtrNotNull<LoginPacketC2SHandler> _handler;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<ServerConnection>> _connections;
    };
}

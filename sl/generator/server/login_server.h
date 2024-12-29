#pragma once
#include "shared/network/server/server.h"
#include "sl/generator/server/server_type.h"
#include "sl/generator/server/client/game_client_id.h"

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
        LoginServer(const ServiceLocator& serviceLocator, execution::AsioExecutor& executor);

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        auto PublishGameClientId() -> game_client_id_type;

        auto FindConnection(session::id_type id) -> ServerConnection*;

    private:
        std::atomic<int64_t> _nextGameClientId = 0;

        SharedPtrNotNull<LoginPacketC2SHandler> _handler;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<ServerConnection>> _connections;
    };
}

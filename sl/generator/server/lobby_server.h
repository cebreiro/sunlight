#pragma once
#include "shared/network/server/server.h"
#include "shared/snowflake/snowflake.h"
#include "sl/generator/server/server_type.h"

namespace sunlight
{
    class ServerConnection;
    class LobbyPacketC2SHandler;
}

namespace sunlight
{
    class LobbyServer final : public Server
    {
    public:
        static constexpr ServerType TYPE = ServerType::Lobby;

    public:
        LobbyServer(const ServiceLocator& serviceLocator, execution::AsioExecutor& executor);

        bool StartUp(uint16_t listenPort) override;

        auto PublishCharacterId() -> int64_t;
        auto PublishItemId() -> int64_t;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        SharedPtrNotNull<LobbyPacketC2SHandler> _handler;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<ServerConnection>> _connections;

        std::optional<SharedSnowflake<>> _characterIdPublisher;
        std::optional<SharedSnowflake<>> _itemIdPublisher;
    };
}

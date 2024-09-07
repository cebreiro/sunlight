#pragma once
#include "shared/network/server/server.h"
#include "shared/snowflake/snowflake.h"
#include "sl/emulator/server/server_type.h"
#include "sl/emulator/service/database/dto/lobby_character.h"

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
        explicit LobbyServer(execution::AsioExecutor& executor);

        void Initialize(ServiceLocator& serviceLocator) override;

        auto PublishCharacterId() -> int64_t;
        auto PublishItemId() -> int64_t;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        auto FindConnection(session::id_type id) -> ServerConnection*;

    private:
        ServiceLocator _serviceLocator;

        SharedPtrNotNull<LobbyPacketC2SHandler> _handler;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<ServerConnection>> _connections;

        std::optional<SharedSnowflake<>> _characterIdPublisher;
        std::optional<SharedSnowflake<>> _itemIdPublisher;
    };
}

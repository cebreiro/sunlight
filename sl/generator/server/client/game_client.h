#pragma once
#include "sl/generator/server/server_type.h"
#include "sl/generator/server/server_connection.h"
#include "sl/generator/server/client/game_client_id.h"
#include "sl/generator/server/client/game_client_state.h"

namespace sunlight
{
    class AuthenticationToken;
    class Session;
    class IPacketCodec;
}

namespace sunlight
{
    class GameClient final : public std::enable_shared_from_this<GameClient>
    {
    public:
        GameClient() = delete;
        GameClient(const GameClient& other) = delete;
        GameClient(GameClient&& other) noexcept = delete;
        GameClient& operator=(const GameClient& other) = delete;
        GameClient& operator=(GameClient&& other) noexcept = delete;

        GameClient(ServiceLocator serviceLocator, game_client_id_type id);
        ~GameClient();

        bool IsConnectedTo(ServerType type) const;

        void Send(ServerType type, Buffer buffer);
        void Send(ServerType type, std::vector<Buffer> buffers);

        void Disconnect();
        void Disconnect(ServerType type);

        auto GetId() const -> game_client_id_type;
        auto GetState() const -> GameClientState;
        auto GetAuthenticationToken() const -> const std::shared_ptr<AuthenticationToken>&;
        auto GetCid() const -> int64_t;

        void SetState(GameClientState state);
        void SetConnection(ServerType type, std::shared_ptr<ServerConnection> conn);
        void SetAuthenticationToken(std::shared_ptr<AuthenticationToken> token);
        void SetCid(int64_t id);

    private:
        auto GetConnection(ServerType type) -> std::shared_ptr<ServerConnection>&;
        auto GetConnection(ServerType type) const -> const std::shared_ptr<ServerConnection>&;

    private:
        ServiceLocator _serviceLocator;
        game_client_id_type _id = game_client_id_type::Default();
        GameClientState _state = GameClientState::LoginConnected;
        std::shared_ptr<AuthenticationToken> _authenticationToken;
        int64_t _cid = -1;

        mutable std::shared_mutex _connectionMutex;
        std::array<std::shared_ptr<ServerConnection>, ServerTypeMeta::GetSize()> _connections = {};
    };
}

namespace fmt
{
    template <>
    struct formatter<sunlight::GameClient> : formatter<std::string>
    {
        auto format(const sunlight::GameClient& client, format_context& context) const
            -> format_context::iterator
        {
            return formatter<std::string>::format(
                fmt::format("{{ id: {}, state: {} }}",
                    client.GetId(), ToString(client.GetState())), context);
        }
    };
}

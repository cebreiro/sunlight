#include "game_client.h"

#include "shared/network/session/session.h"
#include "sl/generator/server/server_constant.h"


namespace sunlight
{
    GameClient::GameClient(ServiceLocator locator, game_client_id_type id)
        : _serviceLocator(std::move(locator))
        , _id(id)
    {
    }

    GameClient::~GameClient()
    {
    }

    bool GameClient::IsConnectedTo(ServerType type) const
    {
        std::shared_lock lock(_connectionMutex);

        return GetConnection(type) != nullptr;
    }

    void GameClient::Send(ServerType type, Buffer buffer)
    {
        std::shared_lock lock(_connectionMutex);

        const std::shared_ptr<ServerConnection>& connection = GetConnection(type);
        if (!connection)
        {
            return;
        }

        connection->Send(std::move(buffer));
    }

    void GameClient::Send(ServerType type, std::vector<Buffer> buffers)
    {
        std::shared_lock lock(_connectionMutex);

        const std::shared_ptr<ServerConnection>& connection = GetConnection(type);
        if (!connection)
        {
            return;
        }

        connection->Send(std::move(buffers));
    }

    void GameClient::Disconnect()
    {
        std::shared_lock lock(_connectionMutex);
        for (std::shared_ptr<ServerConnection>& connection : _connections)
        {
            if (connection)
            {
                connection->Stop();
                connection = {};
            }
        }
    }

    void GameClient::Disconnect(ServerType type)
    {
        std::shared_ptr<ServerConnection> connection;

        {
            std::unique_lock lock(_connectionMutex);

            connection = std::move(GetConnection(type));
        }

        connection->Stop();
    }

    auto GameClient::GetId() const -> game_client_id_type
    {
        return _id;
    }

    auto GameClient::GetState() const -> GameClientState
    {
        return _state;
    }

    auto GameClient::GetAuthenticationToken() const -> const std::shared_ptr<AuthenticationToken>&
    {
        return _authenticationToken;
    }

    auto GameClient::GetCid() const -> int64_t
    {
        return _cid;
    }

    void GameClient::SetState(GameClientState state)
    {
        _state = state;
    }

    void GameClient::SetConnection(ServerType type, std::shared_ptr<ServerConnection> conn)
    {
        std::unique_lock lock(_connectionMutex);

        GetConnection(type) = std::move(conn);
    }

    void GameClient::SetAuthenticationToken(std::shared_ptr<AuthenticationToken> token)
    {
        _authenticationToken = std::move(token);
    }

    void GameClient::SetCid(int64_t id)
    {
        _cid = id;
    }

    auto GameClient::GetConnection(ServerType type) -> std::shared_ptr<ServerConnection>&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_connections));

        return _connections[index];
    }

    auto GameClient::GetConnection(ServerType type) const -> const std::shared_ptr<ServerConnection>&
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_connections));

        return _connections[index];
    }
}

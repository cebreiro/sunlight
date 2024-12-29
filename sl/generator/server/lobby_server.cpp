#include "lobby_server.h"

#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/execution/executor/impl/asio_strand.h"
#include "shared/network/session/session.h"
#include "sl/generator/server/server_connection.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/client/game_client_storage.h"
#include "sl/generator/server/packet/codec/lobby_packet_codec.h"
#include "sl/generator/server/packet/handler/lobby_packet_c2s_handler.h"
#include "sl/generator/service/authentication/authentication_service.h"
#include "sl/generator/service/snowflake/snowflake_service.h"

namespace sunlight
{
    LobbyServer::LobbyServer(const ServiceLocator& serviceLocator, execution::AsioExecutor& executor)
        : Server(serviceLocator, std::string(GetName()), executor)
        , _handler(std::make_shared<LobbyPacketC2SHandler>(serviceLocator, *this))
    {
    }

    bool LobbyServer::StartUp(uint16_t listenPort)
    {
        SnowflakeService& snowflakeService = GetServiceLocator().Get<SnowflakeService>();

        const std::optional<int32_t> characterKey = snowflakeService.Publish(SnowflakeCategory::Character).Get();
        assert(characterKey);

        const std::optional<int32_t> itemKey = snowflakeService.Publish(SnowflakeCategory::Item).Get();
        assert(itemKey);

        _characterIdPublisher.emplace(*characterKey);
        _itemIdPublisher.emplace(*itemKey);

        return Server::StartUp(listenPort);
    }

    auto LobbyServer::PublishCharacterId() -> int64_t
    {
        assert(_characterIdPublisher.has_value());

        return static_cast<int64_t>(_characterIdPublisher->Generate());
    }

    auto LobbyServer::PublishItemId() -> int64_t
    {
        assert(_itemIdPublisher.has_value());

        return static_cast<int64_t>(_itemIdPublisher->Generate());
    }

    void LobbyServer::OnAccept(Session& session)
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] accept session. session: {}",
                GetName(), session));

        auto connection = std::make_shared<ServerConnection>(GetServiceLocator(), TYPE,
            session.shared_from_this(),
            std::make_shared<Strand>(GetExecutor().SharedFromThis()),
            std::make_shared<LobbyPacketCodec>(),
            _handler);
        {
            decltype(_connections)::accessor accessor;

            if (_connections.insert(accessor, session.GetId()))
            {
                accessor->second = connection;
            }
            else
            {
                assert(false);

                session.Close();

                return;
            }
        }

        connection->Start();
    }

    void LobbyServer::OnReceive(Session& session, Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        ServerConnection* connection = [this, id = session.GetId()]() -> ServerConnection*
            {
                decltype(_connections)::accessor accessor;

                if (_connections.find(accessor, id))
                {
                    return accessor->second.get();
                }

                return nullptr;
            }();

        if (!connection)
        {
            SUNLIGHT_LOG_ERROR(GetServiceLocator(),
                fmt::format("[{}] fail to find client. session: {}",
                    GetName(), session.GetId()));

            session.Close();

            return;
        }

        connection->Receive(std::move(buffer));
    }

    void LobbyServer::OnError(Session& session, const boost::system::error_code& error)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] session io error. session: {}, error: {}",
                GetName(), session, error.message()));

        std::shared_ptr<ServerConnection> connection;
        {
            decltype(_connections)::accessor accessor;

            if (_connections.find(accessor, session.GetId()))
            {
                connection = std::move(accessor->second);

                _connections.erase(accessor);
            }
        }

        if (!connection)
        {
            return;
        }

        if (GameClient* client = connection->GetGameClientPtr(); client)
        {
            client->SetConnection(TYPE, std::shared_ptr<ServerConnection>{});

            switch (client->GetState())
            {
            case GameClientState::LobbyAuthenticated:
            case GameClientState::LobbyAndZoneConnecting:
            case GameClientState::LobbyAndZoneAuthenticated:
            {
                (void)GetServiceLocator().Get<GameClientStorage>().Remove(client->GetId());

                if (auto token = client->GetAuthenticationToken(); token)
                {
                    GetServiceLocator().Get<AuthenticationService>().Remove(std::move(token));
                }
            }
            break;
            default:
                SUNLIGHT_LOG_CRITICAL(GetServiceLocator(),
                    fmt::format("[{}] invalid game client state. session: {}, client_id: {}, state: {}",
                        GetName(), session, client->GetId(), ToString(client->GetState())));
            }
        }

        connection->Stop();
        connection->SetGameClient(std::shared_ptr<GameClient>());
    }

    auto LobbyServer::GetName() -> std::string_view
    {
        return "lobby_server";
    }
}

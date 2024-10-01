#include "login_server.h"

#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/execution/executor/impl/asio_strand.h"
#include "shared/network/session/session.h"
#include "sl/emulator/server/server_connection.h"
#include "sl/emulator/server/server_constant.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/client/game_client_storage.h"
#include "sl/emulator/server/packet/creator/login_packet_s2c_creator.h"
#include "sl/emulator/server/packet/codec/login_packet_codec.h"
#include "sl/emulator/server/packet/handler/login_packet_c2s_handler.h"
#include "sl/emulator/service/authentication/authentication_service.h"

namespace sunlight
{
    LoginServer::LoginServer(execution::AsioExecutor& executor)
        : Server(std::string(GetName()), executor)
    {
    }

    void LoginServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
        _handler = std::make_shared<LoginPacketC2SHandler>(serviceLocator);
    }

    void LoginServer::OnAccept(Session& session)
    {
        SUNLIGHT_LOG_INFO(_serviceLocator,
            fmt::format("[{}] accept session. session: {}",
                GetName(), session));

        auto connection = std::make_shared<ServerConnection>(_serviceLocator, TYPE,
            session.shared_from_this(),
            std::make_shared<Strand>(GetExecutor().SharedFromThis()),
            std::make_shared<LoginPacketCodec>(ServerConstant::LOGIN_FIRST_KEY, ServerConstant::LOGIN_SECOND_KEY),
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

        auto client = std::make_shared<GameClient>(_serviceLocator, PublishGameClientId());
        client->SetState(GameClientState::LoginConnected);

        client->SetConnection(TYPE, connection);
        connection->SetGameClient(client);

        GameClientStorage& clientStorage = _serviceLocator.Get<GameClientStorage>();

        [[maybe_unused]]
        const bool inserted = clientStorage.Add(client);
        assert(inserted);

        connection->Start();

        Buffer body = LoginPacketS2CCreator::CreateHello(ServerConstant::LOGIN_FIRST_KEY, ServerConstant::LOGIN_SECOND_KEY);
        Buffer head = connection->MakePacketHeader(body);

        head.MergeBack(std::move(body));

        session.Send(std::move(head));
    }

    void LoginServer::OnReceive(Session& session, Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        ServerConnection* connection = FindConnection(session.GetId());

        if (!connection)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find client. session: {}",
                    GetName(), session.GetId()));

            session.Close();

            return;
        }

        connection->Receive(std::move(buffer));
    }

    void LoginServer::OnError(Session& session, const boost::system::error_code& error)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        SUNLIGHT_LOG_INFO(_serviceLocator,
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
            case GameClientState::LoginConnected:
            case GameClientState::LoginAuthenticated:
            {
                [[maybe_unused]]
                const bool removed = _serviceLocator.Get<GameClientStorage>().Remove(client->GetId());
                assert(removed);

                if (auto token = client->GetAuthenticationToken(); token)
                {
                    _serviceLocator.Get<AuthenticationService>().Remove(std::move(token));
                }
            }
            break;
            case GameClientState::LoginToLobby:
            {
                // do nothing
            }
            break;
            default:
                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] invalid game client state. session: {}, client_id: {}, state: {}",
                        GetName(), session, client->GetId(), ToString(client->GetState())));
            }
        }

        connection->Stop();
        connection->SetGameClient(std::shared_ptr<GameClient>());
    }

    auto LoginServer::GetName() -> std::string_view
    {
        return "login_server";
    }

    auto LoginServer::PublishGameClientId() -> game_client_id_type
    {
        const int64_t id = _nextGameClientId.fetch_add(1);

        return game_client_id_type(id);
    }

    auto LoginServer::FindConnection(session::id_type id) -> ServerConnection*
    {
        decltype(_connections)::accessor accessor;

        if (_connections.find(accessor, id))
        {
            return accessor->second.get();
        }

        return nullptr;
    }
}

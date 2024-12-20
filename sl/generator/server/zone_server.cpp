#include "zone_server.h"

#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/execution/executor/impl/asio_strand.h"
#include "shared/network/session/session.h"
#include "sl/generator/game/zone/zone.h"
#include "sl/generator/server/server_connection.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/client/game_client_state.h"
#include "sl/generator/server/client/game_client_storage.h"
#include "sl/generator/server/packet/codec/zone_packet_codec.h"
#include "sl/generator/server/packet/handler/zone_packet_c2s_handler.h"
#include "sl/generator/service/authentication/authentication_service.h"

namespace sunlight
{
    ZoneServer::ZoneServer(execution::AsioExecutor& executor, execution::IExecutor& gameExecutor, int8_t worldId, int32_t zoneId)
        : Server(std::string(GetName()), executor)
        , _gameExecutor(gameExecutor.SharedFromThis())
        , _worldId(worldId)
        , _zoneId(zoneId)
    {
    }

    ZoneServer::~ZoneServer()
    {
    }

    void ZoneServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
        _handler = std::make_shared<ZonePacketC2SHandler>(serviceLocator, *this);
        _zone = (std::make_shared<Zone>(serviceLocator, *_gameExecutor, _worldId, _zoneId));
        _zone->Initialize();
    }

    void ZoneServer::StartUp(uint16_t listenPort)
    {
        Server::StartUp(listenPort);

        _zone->Start();
    }

    auto ZoneServer::GetWorldId() const -> int8_t
    {
        return _worldId;
    }

    auto ZoneServer::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto ZoneServer::GetZone() -> Zone&
    {
        assert(_zone);

        return *_zone;
    }

    auto ZoneServer::GetZone() const -> const Zone&
    {
        assert(_zone);

        return *_zone;
    }

    void ZoneServer::OnAccept(Session& session)
    {
        SUNLIGHT_LOG_INFO(_serviceLocator,
            fmt::format("[{}] accept session. session: {}",
                GetName(), session));

        auto connection = std::make_shared<ServerConnection>(_serviceLocator, TYPE,
            session.shared_from_this(),
            std::make_shared<Strand>(GetExecutor().SharedFromThis()),
            std::make_shared<ZonePacketCodec>(),
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

    void ZoneServer::OnReceive(Session& session, Buffer buffer)
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
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find client. session: {}",
                    GetName(), session.GetId()));

            session.Close();

            return;
        }

        connection->Receive(std::move(buffer));
    }

    void ZoneServer::OnError(Session& session, const boost::system::error_code& error)
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
            }

            _connections.erase(accessor);
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
            case GameClientState::LobbyAndZoneAuthenticated:
            {
                (void)_serviceLocator.Get<GameClientStorage>().Remove(client->GetId());

                if (auto token = client->GetAuthenticationToken(); token)
                {
                    _serviceLocator.Get<AuthenticationService>().Remove(std::move(token));
                }

                _zone->HandleClientDisconnect(client->GetId());
            }
            break;
            case GameClientState::ZoneChaning:
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

    auto ZoneServer::GetName() -> std::string_view
    {
        return "zone_server";
    }
}

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
    ZoneServer::ZoneServer(const ServiceLocator& serviceLocator, execution::AsioExecutor& executor, Zone& zone)
        : Server(serviceLocator, fmt::format("zone_server_{}_{}", zone.GetWorldId(), zone.GetId()), executor)
        , _zone(zone)
        , _handler(std::make_shared<ZonePacketC2SHandler>(serviceLocator, *this))
    {
    }

    ZoneServer::~ZoneServer()
    {
    }

    auto ZoneServer::GetZoneId() const -> int32_t
    {
        return _zone.GetId();
    }

    auto ZoneServer::GetZone() -> Zone&
    {
        return _zone;
    }

    auto ZoneServer::GetZone() const -> const Zone&
    {
        return _zone;
    }

    void ZoneServer::OnAccept(Session& session)
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] accept session. session: {}",
                GetName(), session));

        auto connection = std::make_shared<ServerConnection>(GetServiceLocator(), TYPE,
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
            SUNLIGHT_LOG_ERROR(GetServiceLocator(),
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

        SUNLIGHT_LOG_INFO(GetServiceLocator(),
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
                (void)GetServiceLocator().Get<GameClientStorage>().Remove(client->GetId());

                if (auto token = client->GetAuthenticationToken(); token)
                {
                    GetServiceLocator().Get<AuthenticationService>().Remove(std::move(token));
                }

                _zone.HandleClientDisconnect(client->GetId());
            }
            break;
            case GameClientState::ZoneChaning:
            {
                // do nothing
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
}

#include "zone_packet_c2s_handler.h"

#include "shared/network/session/session.h"
#include "sl/generator/game/zone/zone.h"
#include "sl/generator/server/zone_server.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/client/game_client_storage.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"
#include "sl/generator/service/authentication/authentication_service.h"
#include "sl/generator/service/database/database_service.h"

namespace sunlight
{
    ZonePacketC2SHandler::ZonePacketC2SHandler(const ServiceLocator& serviceLocator, ZoneServer& zoneServer)
        : _serviceLocator(serviceLocator)
        , _zoneServer(zoneServer)
        , _name(fmt::format("zone_packet_handler_{}", _zoneServer.GetZoneId()))
    {
    }

    auto ZonePacketC2SHandler::HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(connection.GetStrand()));

        auto reader = std::make_unique<SlPacketReader>(std::move(packet));

        const ZonePacketC2S opcode = reader->Read<ZonePacketC2S>();

        const GameClient* client = connection.GetGameClientPtr();
        const GameClientState state = client ? client->GetState() : GameClientState::None;

        bool handled = false;

        switch (state)
        {
        case GameClientState::None:
        {
            if (opcode == ZonePacketC2S::NMC_LOGIN)
            {
                handled = true;

                co_await HandleLogin(connection, *reader);
            }
        }
        break;
        case GameClientState::LobbyAndZoneAuthenticated:
        {
            switch (opcode)
            {
            case ZonePacketC2S::NMC_LOGOUT:
            {
                handled = true;

                SUNLIGHT_LOG_INFO(_serviceLocator,
                    fmt::format("[{}] client logout request. zone: {}",
                        GetName(), _zoneServer.GetZone().GetId()));

                if (const GameClient* clientPtr = connection.GetGameClientPtr(); clientPtr)
                {
                    _zoneServer.GetZone().LogoutPlayer(clientPtr->GetId());
                }
            }
            break;
            default:
            {
                handled = true;

                Delegate(connection, opcode, std::move(reader));
            }
            }
        }
        break;
        }

        if (!handled)
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to handle packet. session: {}, client_state: {}, opcode: {}",
                    GetName(), connection.GetSession().GetId(), ToString(state), ToString(opcode)));
        }

        co_return;
    }

    auto ZonePacketC2SHandler::GetName() const -> const std::string&
    {
        return _name;
    }

    auto ZonePacketC2SHandler::HandleLogin(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        // unk
        (void)reader.ReadObject();

        // SlPacket format
        BufferReader bufferReader = reader.ReadObject();
        bufferReader.Skip(2);
        bufferReader.Skip(2);

        bufferReader.Skip(1);
        const std::string playerName = bufferReader.ReadString();

        bufferReader.Skip(1);
        const uint32_t auth = bufferReader.Read<uint32_t>();

        std::shared_ptr<AuthenticationToken> token = co_await _serviceLocator.Get<AuthenticationService>().FindByPlayerName(auth, playerName);
        if (!token)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find auth_token. session: {}",
                    GetName(), connection.GetSession().GetId()));

            connection.Stop();

            co_return;
        }

        std::shared_ptr<GameClient> client = _serviceLocator.Get<GameClientStorage>().Find(token->GetClientId());
        if (!client)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find game_client. session: {}, client_id: {}",
                    GetName(), connection.GetSession().GetId(), token->GetClientId()));

            connection.Stop();

            co_return;
        }

        if (client->GetState() != GameClientState::LobbyAndZoneConnecting &&
            client->GetState() != GameClientState::ZoneChaning)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] invalid request. session: {}, client_id: {}, state: {}",
                    GetName(), connection.GetSession().GetId(), token->GetClientId(), ToString(client->GetState())));

            connection.Stop();

            co_return;
        }

        client->SetConnection(ZoneServer::TYPE, connection.shared_from_this());
        client->SetState(GameClientState::LobbyAndZoneAuthenticated);

        connection.SetGameClient(client);

        std::optional<db::dto::Character> dto = co_await _serviceLocator.Get<DatabaseService>().GetCharacter(client->GetCid());
        if (!dto.has_value())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find character. session: {}, client_id: {}, cid: {}",
                    GetName(), connection.GetSession().GetId(), token->GetClientId(), client->GetCid()));

            connection.Stop();

            co_return;
        }

        const bool result = co_await _zoneServer.GetZone().SpawnPlayer(client, *dto);
        (void)result;
    }

    void ZonePacketC2SHandler::Delegate(ServerConnection& connection, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader) const
    {
        GameClient* client = connection.GetGameClientPtr();
        if (!client)
        {
            assert(false);

            return;
        }

        const std::shared_ptr<AuthenticationToken>& authToken = client->GetAuthenticationToken();
        if (!authToken)
        {
            assert(false);

            return;
        }

        _zoneServer.GetZone().HandleNetworkMessage(client->GetId(), opcode, std::move(reader));
    }
}

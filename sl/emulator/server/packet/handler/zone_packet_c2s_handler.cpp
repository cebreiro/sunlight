#include "zone_packet_c2s_handler.h"

#include "shared/network/session/session.h"
#include "sl/emulator/game/zone/zone.h"
#include "sl/emulator/server/zone_server.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/client/game_client_storage.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"
#include "sl/emulator/service/authentication/authentication_service.h"
#include "sl/emulator/service/database/database_service.h"

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
            handled = true;

            Delegate(connection, opcode, std::move(reader));
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

        BufferReader bufferReader = reader.ReadObject();
        bufferReader.Skip(5); // maybe... some object serialize/deserialize protocol

        const std::string str = bufferReader.ReadString();

        const std::optional<AuthenticationToken::Key> opt = AuthenticationToken::Key::From(str);
        if (!opt.has_value())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to create auth_token_key from string. session: {}, str: {}",
                    GetName(), connection.GetSession().GetId(), str));

            connection.Stop();

            co_return;
        }

        std::shared_ptr<AuthenticationToken> token = co_await _serviceLocator.Get<AuthenticationService>().Find(*opt);
        if (!token)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find auth_token. session: {}, key: {}",
                    GetName(), connection.GetSession().GetId(), opt->ToString()));

            connection.Stop();

            co_return;
        }

        std::shared_ptr<GameClient> client = _serviceLocator.Get<GameClientStorage>().Find(token->GetClientId());
        if (!client)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find game_client. session: {}, key: {}, client_id: {}",
                    GetName(), connection.GetSession().GetId(), opt->ToString(), token->GetClientId()));

            connection.Stop();

            co_return;
        }

        if (client->GetState() != GameClientState::LobbyAndZoneConnecting)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] invalid request. session: {}, key: {}, client_id: {}, state: {}",
                    GetName(), connection.GetSession().GetId(), opt->ToString(), token->GetClientId(), ToString(client->GetState())));

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
                fmt::format("[{}] fail to find character. session: {}, key: {}, client_id: {}, cid: {}",
                    GetName(), connection.GetSession().GetId(), opt->ToString(), token->GetClientId(), client->GetCid()));

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

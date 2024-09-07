#include "zone_packet_c2s_handler.h"

#include "shared/network/session/session.h"
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
        Bind(GameClientState::None, ZonePacketC2S::NMC_LOGIN, &ZonePacketC2SHandler::HandleLogin);
    }

    auto ZonePacketC2SHandler::HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(connection.GetStrand()));

        SlPacketReader reader(packet);

        const ZonePacketC2S opcode = reader.Read<ZonePacketC2S>();

        const auto iter = _handlers.find(opcode);
        if (iter == _handlers.end())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to find handler. session: {}, opcode: {}, packet: {}",
                    GetName(), connection.GetSession().GetId(), ToString(opcode), packet.ToString()));

            co_return;
        }

        const GameClient* client = connection.GetGameClient();
        const GameClientState state = client ? client->GetState() : GameClientState::None;

        const auto [begin, end] = _allows.equal_range(state);
        auto range = std::ranges::subrange(begin, end) | std::views::values;

        if (!std::ranges::contains(range, opcode))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to handle packet. not allowed handler. session: {}, client_state: {}, packet: {}",
                    GetName(), connection.GetSession().GetId(), ToString(opcode), packet.ToString()));

            co_return;
        }

        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await iter->second(connection, reader);

        co_return;
    }

    auto ZonePacketC2SHandler::GetName() const -> const std::string&
    {
        return _name;
    }

    void ZonePacketC2SHandler::Bind(GameClientState state, ZonePacketC2S opcode,
        Future<void>(ZonePacketC2SHandler::* handler)(ServerConnection&, SlPacketReader&) const)
    {
        _handlers[opcode] = std::bind_front(handler, this);

        _allows.emplace(state, opcode);
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

        client->SetConnection(ZoneServer::TYPE, connection.shared_from_this());
        client->SetState(GameClientState::LobbyAndZoneAuthenticated);

        connection.SetGameClient(client.get());

        std::optional<db::dto::Character> dto = co_await _serviceLocator.Get<DatabaseService>().GetCharacter(client->GetCid());
        if (!dto.has_value())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to find character. session: {}, key: {}, client_id: {}, cid: {}",
                    GetName(), connection.GetSession().GetId(), opt->ToString(), token->GetClientId(), client->GetCid()));

            connection.Stop();

            co_return;
        }

        ::system("pause");
    }
}

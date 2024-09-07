#include "lobby_packet_c2s_handler.h"

#include <boost/locale.hpp>
#include <boost/scope_exit.hpp>
#include <boost/scope/scope_exit.hpp>

#include "shared/network/session/session.h"
#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/server/lobby_server.h"
#include "sl/emulator/server/server_connection.h"
#include "sl/emulator/server/server_constant.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/client/game_client_storage.h"
#include "sl/emulator/server/packet/creator/lobby_packet_s2c_creator.h"
#include "sl/emulator/server/packet/dto/lobby_c2s_character_create.h"
#include "sl/emulator/server/packet/dto/lobby_s2c_endpoint.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"
#include "sl/emulator/service/authentication/authentication_service.h"
#include "sl/emulator/service/authentication/authentication_token.h"
#include "sl/emulator/service/database/database_service.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gateway/gateway_service.h"
#include "sl/emulator/service/unique_name/unique_name_service.h"

namespace sunlight
{
    using lobby_characters_type = std::vector<dto::LobbyCharacter>;

    LobbyPacketC2SHandler::LobbyPacketC2SHandler(const ServiceLocator& serviceLocator, LobbyServer& lobbyServer)
        : _serviceLocator(serviceLocator)
        , _lobbyServer(lobbyServer)
    {
        Bind(GameClientState::None, LobbyPacketC2S::ClientVersion, &LobbyPacketC2SHandler::HandleVersionCheck);
        Bind(GameClientState::None, LobbyPacketC2S::Authentication, &LobbyPacketC2SHandler::HandleAuthentication);
        Bind(GameClientState::LobbyAuthenticated, LobbyPacketC2S::CharacterList, &LobbyPacketC2SHandler::HandleCharacterList);
        Bind(GameClientState::LobbyAuthenticated, LobbyPacketC2S::CharacterNameCheck, &LobbyPacketC2SHandler::HandleCharacterNameCheck);
        Bind(GameClientState::LobbyAuthenticated, LobbyPacketC2S::CharacterCreate, &LobbyPacketC2SHandler::HandleCharacterCreate);
        Bind(GameClientState::LobbyAuthenticated, LobbyPacketC2S::CharacterDelete, &LobbyPacketC2SHandler::HandleCharacterDelete);
        Bind(GameClientState::LobbyAuthenticated, LobbyPacketC2S::CharacterSelect, &LobbyPacketC2SHandler::HandleCharacterSelect);
    }

    auto LobbyPacketC2SHandler::HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(connection.GetStrand()));

        SlPacketReader reader(packet);

        const LobbyPacketC2S opcode = reader.Read<LobbyPacketC2S>();

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

    auto LobbyPacketC2SHandler::GetName() const -> std::string_view
    {
        return "lobby_packet_c2s_handler";
    }

    void LobbyPacketC2SHandler::Bind(GameClientState state, LobbyPacketC2S opcode,
        Future<void>(LobbyPacketC2SHandler::* handler)(ServerConnection&, SlPacketReader&) const)
    {
        _handlers[opcode] = std::bind_front(handler, this);

        _allows.emplace(state, opcode);
    }

    auto LobbyPacketC2SHandler::HandleVersionCheck(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        const int32_t version = reader.Read<int32_t>();
        const bool success = version == ServerConstant::GAME_VERSION;

        connection.Send(LobbyPacketS2CCreator::CreateClientVersionCheckResult(success));

        co_return;
    }

    auto LobbyPacketC2SHandler::HandleAuthentication(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        const uint32_t high = reader.Read<uint32_t>();
        const uint32_t low = reader.Read<uint32_t>();

        [[maybe_unused]]
        const auto unk = reader.ReadInt64();

        bool success = false;

        do
        {
            const AuthenticationToken::Key key(low, high);
            AuthenticationService& authService = _serviceLocator.Get<AuthenticationService>();

            std::shared_ptr<AuthenticationToken> token = co_await authService.Find(key);
            if (!token)
            {
                break;
            }

            std::shared_ptr<GameClient> client = _serviceLocator.Get<GameClientStorage>().Find(token->GetClientId());
            if (!client)
            {
                co_await authService.Remove(std::move(token));

                break;
            }

            client->SetConnection(LobbyServer::TYPE, connection.shared_from_this());
            client->SetState(GameClientState::LobbyAuthenticated);

            connection.SetGameClient(client.get());

            success = true;

        } while (false);

        connection.Send(LobbyPacketS2CCreator::CreateAuthenticationResult(success, "unknown_string..."));

        if (!success)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to auth. session: {}, key: {{ low: {}, high: {} }}",
                    GetName(), connection.GetSession().GetId(), low, high));

            connection.Stop();
        }

        co_return;
    }

    auto LobbyPacketC2SHandler::HandleCharacterList(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        (void)reader;

        GameClient* client = connection.GetGameClient();
        if (!client)
        {
            assert(false);

            co_return;
        }

        const std::shared_ptr<AuthenticationToken>& authToken = client->GetAuthenticationToken();
        if (!authToken)
        {
            assert(false);

            co_return;
        }

        std::any& storage = connection.GetStorage();

        std::optional<std::vector<dto::LobbyCharacter>> lobbyCharacters =
            co_await _serviceLocator.Get<DatabaseService>().GetLobbyCharacters(authToken->GetAccountId(), authToken->GetSid());
        if (!lobbyCharacters.has_value())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to get lobby characters. session: {}, auth_token: {}",
                    GetName(), connection.GetSession().GetId(), authToken->GetKey().ToString()));

            connection.Stop();

            co_return;
        }

        storage = std::move(*lobbyCharacters);

        connection.Send(LobbyPacketS2CCreator::CreateCharacterList(
            _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider(),
            std::any_cast<const lobby_characters_type&>(storage)));
    }

    auto LobbyPacketC2SHandler::HandleCharacterNameCheck(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        GameClient* client = connection.GetGameClient();
        if (!client)
        {
            assert(false);

            co_return;
        }

        const std::shared_ptr<AuthenticationToken>& authToken = client->GetAuthenticationToken();
        if (!authToken)
        {
            assert(false);

            co_return;
        }

        const std::string name = reader.ReadString();

        const bool usable = !co_await _serviceLocator.Get<UniqueNameService>().Has(
            boost::locale::conv::to_utf<wchar_t>(name, ServerConstant::TEXT_ENCODING));

        connection.Send(LobbyPacketS2CCreator::CreateCharacterNameCheckResult(usable, name));
    }

    auto LobbyPacketC2SHandler::HandleCharacterCreate(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        GameClient* client = connection.GetGameClient();
        if (!client)
        {
            assert(false);

            co_return;
        }

        const std::shared_ptr<AuthenticationToken>& authToken = client->GetAuthenticationToken();
        if (!authToken)
        {
            assert(false);

            co_return;
        }

        BufferReader bufferReader = reader.ReadObject();
        const LobbyC2SCharacterCreate characterCreate = LobbyC2SCharacterCreate::CreateFrom(bufferReader);

        const int8_t slot = static_cast<int8_t>(reader.Read<int32_t>());

        bool success = false;
        UniqueNameService& uniqueNameService = _serviceLocator.Get<UniqueNameService>();

        const auto uniqueNameReserveKey = co_await uniqueNameService.Reserve(
            boost::locale::conv::to_utf<wchar_t>(characterCreate.name, ServerConstant::TEXT_ENCODING));

        if (!uniqueNameReserveKey.has_value())
        {
            connection.Send(LobbyPacketS2CCreator::CreateCharacterCreateResult(false));

            co_return;
        }

        boost::scope::scope_exit exit([&]()
            {
                if (success)
                {
                    (void)uniqueNameService.Commit(*uniqueNameReserveKey);
                    
                }
                else
                {
                    (void)uniqueNameService.Rollback(*uniqueNameReserveKey);
                }
            });

        // TODO: hacking check

        req::CharacterCreate request;
        request.id = _lobbyServer.PublishCharacterId();
        request.aid = authToken->GetAccountId();
        request.sid = authToken->GetSid();
        request.slot = slot;
        request.name = characterCreate.name;
        request.hair = characterCreate.hair;
        request.hairColor = characterCreate.hairColor;
        request.face = characterCreate.face;
        request.skinColor = characterCreate.skinColor;
        request.zone = GameConstant::START_ZONE;
        request.x = GameConstant::START_ZONE_X;
        request.y = GameConstant::START_ZONE_Y;
        request.gender = characterCreate.gender;
        request.str = characterCreate.str;
        request.dex = characterCreate.dex;
        request.accr = characterCreate.accr;
        request.health = characterCreate.health;
        request.intell = characterCreate.intell;
        request.wis = characterCreate.wis;
        request.will = characterCreate.will;
        request.water = characterCreate.water;
        request.fire = characterCreate.fire;
        request.lightning = characterCreate.lightning;
        request.job = characterCreate.job1;

        std::array equipments{
            std::pair{ characterCreate.jacketId, EquipmentPosition::Jacket },
            std::pair{ characterCreate.pantsId, EquipmentPosition::Pants },
            std::pair{ characterCreate.shoesId, EquipmentPosition::Shoes },
        };

        for (const auto [itemId, pos] : equipments)
        {
            request.items.emplace_back(req::ItemCreate{
                .id = _lobbyServer.PublishItemId(),
                .cid = request.id,
                .itemId = itemId,
                .quantity = 1,
                .posType = db::ItemPosType::Equipment,
                .page = static_cast<int8_t>(pos),
                .x = -1,
                .y = -1
                });
        }

        request.skills.emplace_back(req::SkillCreate{
            .cid = request.id,
            .id = 123123,
            .job = request.job,
            .level = 1
            });

        success = co_await _serviceLocator.Get<DatabaseService>().CreateCharacter(request);
        if (!success)
        {
            nlohmann::json j;
            to_json(j, request);

            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to create character. client: {}, request: {}",
                    GetName(), *client, j.dump()));
        }

        connection.Send(LobbyPacketS2CCreator::CreateCharacterCreateResult(success));
    }

    auto LobbyPacketC2SHandler::HandleCharacterDelete(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        bool success = false;

        boost::scope::scope_exit exit([&success, &connection]()
            {
                connection.Send(LobbyPacketS2CCreator::CreateCharacterDeleteResult(success));
            });

        GameClient* client = connection.GetGameClient();
        if (!client)
        {
            assert(false);

            co_return;
        }

        const std::shared_ptr<AuthenticationToken>& authToken = client->GetAuthenticationToken();
        if (!authToken)
        {
            assert(false);

            co_return;
        }

        std::any& storage = connection.GetStorage();
        if (!storage.has_value())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] invalid request. session: {}, auth_token: {}",
                    GetName(), connection.GetSession().GetId(), authToken->GetKey().ToString()));

            connection.Stop();

            co_return;
        }

        const int32_t slot = reader.Read<int32_t>();

        lobby_characters_type& characters = std::any_cast<lobby_characters_type&>(storage);

        const auto iter = std::ranges::find_if(characters, [slot](const dto::LobbyCharacter& character) -> bool
            {
                return character.slot == slot;
            });
        if (iter == characters.end())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] invalid request. fail to find target character. session: {}, auth_token: {}, slot: {}",
                    GetName(), connection.GetSession().GetId(), authToken->GetKey().ToString(), slot));

            connection.Stop();

            co_return;
        }

        success = co_await _serviceLocator.Get<DatabaseService>().DeleteCharacterSoft(iter->cid);
        if (success)
        {
            characters.erase(iter);
        }
        else
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] invalid request. fail to delete target character. session: {}, auth_token: {}, slot: {}",
                    GetName(), connection.GetSession().GetId(), authToken->GetKey().ToString(), slot));

            connection.Stop();
        }

        connection.Send(LobbyPacketS2CCreator::CreateCharacterDeleteResult(success));
    }

    auto LobbyPacketC2SHandler::HandleCharacterSelect(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        GameClient* client = connection.GetGameClient();
        if (!client)
        {
            assert(false);

            co_return;
        }

        const std::shared_ptr<AuthenticationToken>& authToken = client->GetAuthenticationToken();
        if (!authToken)
        {
            assert(false);

            co_return;
        }

        std::any& storage = connection.GetStorage();
        if (!storage.has_value())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] invalid request. session: {}, auth_token: {}",
                    GetName(), connection.GetSession().GetId(), authToken->GetKey().ToString()));

            connection.Stop();

            co_return;
        }

        const int32_t slot = reader.Read<int32_t>();

        lobby_characters_type& characters = std::any_cast<lobby_characters_type&>(storage);

        const auto iter = std::ranges::find_if(characters, [slot](const dto::LobbyCharacter& character) -> bool
            {
                return character.slot == slot;
            });
        if (iter == characters.end())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] invalid request. fail to find target character. session: {}, auth_token: {}, slot: {}",
                    GetName(), connection.GetSession().GetId(), authToken->GetKey().ToString(), slot));

            connection.Stop();

            co_return;
        }

        const auto& opt = co_await _serviceLocator.Get<GatewayService>().FindZone(authToken->GetSid(), iter->zone);
        if (!opt.has_value())
        {
            connection.Send(LobbyPacketS2CCreator::CreateCharacterSelectFail());

            co_return;
        }

        constexpr int32_t unused = 0;
        const std::string& key = authToken->GetKey().ToString();

        client->SetState(GameClientState::LobbyAndZoneConnecting);
        client->SetCid(iter->cid);

        connection.Send(LobbyPacketS2CCreator::CreateCharacterSelectSuccess(unused, key, LobbyS2CEndpoint(opt->first, opt->second)));
    }
}

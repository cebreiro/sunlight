#include "lobby_packet_c2s_handler.h"

#include <boost/locale.hpp>
#include <boost/scope_exit.hpp>
#include <boost/scope/scope_exit.hpp>

#include "shared/network/session/session.h"
#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/contents/item/equipment_position.h"
#include "sl/generator/game/data/sox/item_clothes.h"
#include "sl/generator/server/lobby_server.h"
#include "sl/generator/server/server_connection.h"
#include "sl/generator/server/server_constant.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/client/game_client_storage.h"
#include "sl/generator/server/packet/creator/lobby_packet_s2c_creator.h"
#include "sl/generator/server/packet/dto/lobby_c2s_character_create.h"
#include "sl/generator/server/packet/dto/lobby_s2c_endpoint.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"
#include "sl/generator/service/authentication/authentication_service.h"
#include "sl/generator/service/authentication/authentication_token.h"
#include "sl/generator/service/database/database_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"
#include "sl/generator/service/gamedata/skill/skill_data_provider.h"
#include "sl/generator/service/gateway/gateway_service.h"
#include "sl/generator/service/unique_name/unique_name_service.h"

namespace
{
    bool IsAllowedItemOnCharacterCreation(int32_t itemId)
    {
        switch (itemId)
        {
        case 2311011:
        case 2311021:
        case 2311031:
        case 2111011:
        case 2111021:
        case 2111031:
        case 2111041:
        case 2111051:
        case 2111061:
        case 2111071:
        case 2111081:
        case 2111091:
        case 2111101:
        case 2111111:
        case 2111121:
        case 2211011:
        case 2211021:
        case 2211031:
        case 2211041:
        case 2211051:
        case 2211061:
        case 2211071:
        case 2211081:
        case 2211091:
        case 2211101:
        case 2211111:
        case 2211121:
        case 2411011:
        case 2411021:
        case 2411031:
        case 2411041:
        case 2411051:
        case 2411061:
        case 2411071:
        case 2411081:
        case 2411091:
        case 2411101:
        case 2411111:
        case 2411121:
        case 2121011:
        case 2121021:
        case 2121031:
        case 2121041:
        case 2121051:
        case 2121061:
        case 2121071:
        case 2121081:
        case 2121091:
        case 2121101:
        case 2121111:
        case 2121121:
        case 2221011:
        case 2221021:
        case 2221031:
        case 2221041:
        case 2221051:
        case 2221061:
        case 2221071:
        case 2221081:
        case 2221091:
        case 2221101:
        case 2221111:
        case 2221121:
        case 2421011:
        case 2421021:
        case 2421031:
        case 2421041:
        case 2421051:
        case 2421061:
        case 2421071:
        case 2421081:
        case 2421091:
        case 2421101:
        case 2421111:
        case 2421121:
        case 2131011:
        case 2131021:
        case 2131031:
        case 2131041:
        case 2131051:
        case 2131061:
        case 2131071:
        case 2131081:
        case 2131091:
        case 2131101:
        case 2131111:
        case 2131121:
        case 2231011:
        case 2231021:
        case 2231031:
        case 2231041:
        case 2231051:
        case 2231061:
        case 2231071:
        case 2231081:
        case 2231091:
        case 2231101:
        case 2231111:
        case 2231121:
        case 2431011:
        case 2431021:
        case 2431031:
        case 2431041:
        case 2431051:
        case 2431061:
        case 2431071:
        case 2431081:
        case 2431091:
        case 2431101:
        case 2431111:
        case 2431121:
        case 2141011:
        case 2141021:
        case 2141031:
        case 2141041:
        case 2141051:
        case 2141061:
        case 2141071:
        case 2141081:
        case 2141091:
        case 2811011:
        case 2811021:
        case 2811031:
        case 2611011:
        case 2611021:
        case 2611031:
        case 2611041:
        case 2611051:
        case 2611061:
        case 2611071:
        case 2611081:
        case 2611091:
        case 2611101:
        case 2711011:
        case 2711021:
        case 2711031:
        case 2711041:
        case 2711051:
        case 2711061:
        case 2711071:
        case 2711081:
        case 2711091:
        case 2711101:
        case 2911011:
        case 2911021:
        case 2911031:
        case 2911041:
        case 2911051:
        case 2911061:
        case 2911071:
        case 2911081:
        case 2911091:
        case 2911101:
        case 2621011:
        case 2621021:
        case 2621031:
        case 2621041:
        case 2621051:
        case 2621061:
        case 2621071:
        case 2621081:
        case 2621091:
        case 2621101:
        case 2721011:
        case 2721021:
        case 2721031:
        case 2721041:
        case 2721051:
        case 2721061:
        case 2721071:
        case 2721081:
        case 2721091:
        case 2721101:
        case 2921011:
        case 2921021:
        case 2921031:
        case 2921041:
        case 2921051:
        case 2921061:
        case 2921071:
        case 2921081:
        case 2921091:
        case 2921101:
        case 2631011:
        case 2631021:
        case 2631031:
        case 2631041:
        case 2631051:
        case 2631061:
        case 2631071:
        case 2631081:
        case 2631091:
        case 2631101:
        case 2731011:
        case 2731021:
        case 2731031:
        case 2731041:
        case 2731051:
        case 2731061:
        case 2731071:
        case 2731081:
        case 2731091:
        case 2731101:
        case 2931011:
        case 2931021:
        case 2931031:
        case 2931041:
        case 2931051:
        case 2931061:
        case 2931071:
        case 2931081:
        case 2931091:
        case 2931101:
        case 2641011:
        case 2641021:
        case 2641031:
        case 2641041:
        case 2641051:
        case 2641061:
        case 2641071:
        case 2641081:
        case 2641091:
        case 2641101:
        case 2741011:
        case 2741021:
        case 2741031:
        case 2741041:
        case 2741051:
        case 2741061:
        case 2741071:
        case 2741081:
        case 2741091:
        case 2741101:
        case 2941011:
        case 2941021:
        case 2941031:
        case 2941041:
        case 2941051:
        case 2941061:
        case 2941071:
        case 2941081:
        case 2941091:
        case 2941101:
        case 2141101:
        case 2141111:
        case 2141121:
        case 2241011:
        case 2241021:
        case 2241031:
        case 2241041:
        case 2241051:
        case 2241061:
        case 2241071:
        case 2241081:
        case 2241091:
        case 2241101:
        case 2241111:
        case 2241121:
        case 2441011:
        case 2441021:
        case 2441031:
        case 2441041:
        case 2441051:
        case 2441061:
        case 2441071:
        case 2441081:
        case 2441091:
        case 2441101:
        case 2441111:
        case 2441121:
            return true;
        default:
            break;
        }

        return false;
    }
}

namespace sunlight
{
    using lobby_characters_type = std::vector<db::dto::LobbyCharacter>;

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

        SlPacketReader reader(std::move(packet));

        const LobbyPacketC2S opcode = reader.Read<LobbyPacketC2S>();

        const auto iter = _handlers.find(opcode);
        if (iter == _handlers.end())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to find handler. session: {}, opcode: {}, packet: {}",
                    GetName(), connection.GetSession().GetId(), ToString(opcode), reader.GetBuffer().ToString()));

            co_return;
        }

        const GameClient* client = connection.GetGameClientPtr();
        const GameClientState state = client ? client->GetState() : GameClientState::None;

        const auto [begin, end] = _allows.equal_range(state);
        auto range = std::ranges::subrange(begin, end) | std::views::values;

        if (!std::ranges::contains(range, opcode))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to handle packet. not allowed handler. session: {}, client_state: {}, packet: {}",
                    GetName(), connection.GetSession().GetId(), ToString(opcode), reader.GetBuffer().ToString()));

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

            connection.SetGameClient(std::move(client));

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

        GameClient* client = connection.GetGameClientPtr();
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

        std::optional<std::vector<db::dto::LobbyCharacter>> lobbyCharacters =
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
        GameClient* client = connection.GetGameClientPtr();
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
        GameClient* client = connection.GetGameClientPtr();
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
                connection.Send(LobbyPacketS2CCreator::CreateCharacterCreateResult(success));

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

        const GameDataProvideService& gameDataProvider = _serviceLocator.Get<GameDataProvideService>();
        const ItemDataProvider& itemDataProvider = gameDataProvider.GetItemDataProvider();

        if (std::ranges::any_of(equipments, [&itemDataProvider](const std::pair<int32_t, EquipmentPosition>& pair) -> bool
            {
                if (!IsAllowedItemOnCharacterCreation(pair.first))
                {
                    return true;
                }

                const ItemData* itemData = itemDataProvider.Find(pair.first);
                if (!itemData)
                {
                    return true;
                }

                const sox::ItemClothes* clothesData = itemData->GetClothesData();
                if (!clothesData)
                {
                    return true;
                }

                switch (static_cast<sox::EquipmentType>(clothesData->equipPos))
                {
                case sox::EquipmentType::Jacket:
                {
                    if (pair.second != EquipmentPosition::Jacket)
                    {
                        return true;
                    }
                }
                break;
                case sox::EquipmentType::Pants:
                {
                    if (pair.second != EquipmentPosition::Pants)
                    {
                        return true;
                    }
                }
                break;
                case sox::EquipmentType::Shoes:
                {
                    if (pair.second != EquipmentPosition::Shoes)
                    {
                        return true;
                    }
                }
                break;
                case sox::EquipmentType::Hat:
                case sox::EquipmentType::Gloves:
                case sox::EquipmentType::Necklace:
                case sox::EquipmentType::Ring:
                case sox::EquipmentType::Weapon:
                case sox::EquipmentType::Bullet:
                case sox::EquipmentType::Count:
                case sox::EquipmentType::None:
                    return true;
                }

                return false;
            }))
        {
            // TODO: add cheat log
            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] item creation cheat detected. session: {}, item0: {}, item1: {}, item2: {}",
                    GetName(), connection.GetSession(), equipments[0].first, equipments[1].first, equipments[2].first));

            success = false;

            co_return;
        }

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

        const SkillDataProvider& skillDataProvider = gameDataProvider.GetSkillDataProvider();

        for (const PlayerSkillData& skillData : skillDataProvider.FindByJob(request.job, 1) | notnull::reference)
        {
            request.skills.emplace_back(req::SkillCreate{
                .cid = request.id,
                .id = skillData.index,
                .job = request.job,
                .level = 1
                });
        }

        // item mix skill
        for (const PlayerSkillData& skillData : skillDataProvider.FindByJob(900, 1) | notnull::reference)
        {
            request.skills.emplace_back(req::SkillCreate{
                .cid = request.id,
                .id = skillData.index,
                .job = 900,
                .level = 1
                });
        }

        success = co_await _serviceLocator.Get<DatabaseService>().CreateCharacter(request);
        if (!success)
        {
            nlohmann::json j;
            to_json(j, request);

            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to create character. client: {}, request: {}",
                    GetName(), *client, j.dump()));
        }
    }

    auto LobbyPacketC2SHandler::HandleCharacterDelete(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>
    {
        bool success = false;

        boost::scope::scope_exit exit([&success, &connection]()
            {
                connection.Send(LobbyPacketS2CCreator::CreateCharacterDeleteResult(success));
            });

        GameClient* client = connection.GetGameClientPtr();
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

        const auto iter = std::ranges::find_if(characters, [slot](const db::dto::LobbyCharacter& character) -> bool
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
        GameClient* client = connection.GetGameClientPtr();
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

        const auto iter = std::ranges::find_if(characters, [slot](const db::dto::LobbyCharacter& character) -> bool
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

        authToken->SetPlayerName(iter->name);

        const uint32_t auth = authToken->GetKey().GetLow();
        const std::string& playerName = authToken->GetPlayerName();

        client->SetState(GameClientState::LobbyAndZoneConnecting);
        client->SetCid(iter->cid);

        connection.Send(LobbyPacketS2CCreator::CreateCharacterSelectSuccess(
            auth, playerName, LobbyS2CEndpoint(opt->first, opt->second)));
    }
}

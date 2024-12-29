#include "login_packet_c2s_handler.h"

#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/client/game_client_storage.h"
#include "sl/generator/server/packet/creator/login_packet_s2c_creator.h"
#include "sl/generator/service/authentication/authentication_service.h"
#include "sl/generator/service/database/database_service.h"
#include "sl/generator/service/gateway/gateway_service.h"
#include "sl/generator/service/hash/safe_hash_service.h"

namespace sunlight
{
    LoginPacketC2SHandler::LoginPacketC2SHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
        BindHandler(GameClientState::LoginConnected, LoginPacketC2S::Login, &LoginPacketC2SHandler::HandleLogin);
        BindHandler(GameClientState::LoginAuthenticated, LoginPacketC2S::SelectWorld, &LoginPacketC2SHandler::HandleWorldSelect);
    }

    auto LoginPacketC2SHandler::HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(connection.GetStrand()));

        GameClient* client = connection.GetGameClientPtr();
        assert(client);

        const auto* handlers = [&]() -> const std::unordered_map<LoginPacketC2S, std::function<Future<void>(GameClient&, BufferReader&)>>*
            {
                const auto iter = _handlers.find(client->GetState());

                return iter != _handlers.end() ? &iter->second : nullptr;
            }();

        if (!handlers)
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to find handler. client: {}, packet: {}",
                    GetName(), *client, packet.ToString()));

            co_return;
        }

        BufferReader reader(packet.cbegin(), packet.cend());

        LoginPacketC2S opcode = {};
        reader.Read<LoginPacketC2S>(opcode);

        const auto iter = handlers->find(opcode);
        if (iter == handlers->end())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to find handler. client: {}, opcode: {}, packet: {}",
                    GetName(), *client, ToString(opcode), packet.ToString()));

            co_return;
        }

        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await iter->second(*client, reader);

        co_return;
    }

    auto LoginPacketC2SHandler::GetName() const -> std::string_view
    {
        return "login_packet_c2s_handler";
    }

    void LoginPacketC2SHandler::BindHandler(GameClientState state, LoginPacketC2S opcode,
        Future<void>(LoginPacketC2SHandler::* handler)(GameClient&, BufferReader&) const)
    {
        _handlers[state][opcode] = std::bind_front(handler, this);
    }

    auto LoginPacketC2SHandler::HandleLogin(GameClient& client, BufferReader& reader) const -> Future<void>
    {
        std::string account = ReadString(reader, 14);
        std::string password = ReadString(reader, 16);

        DatabaseService& databaseService = _serviceLocator.Get<DatabaseService>();
        SafeHashService& safeHashService = _serviceLocator.Get<SafeHashService>();

        std::optional<db::dto::Account> dto = co_await databaseService.FindAccount(account);

        if (!dto.has_value())
        {
            std::string encoded = co_await safeHashService.Hash(password);
            constexpr int8_t gmLevel = 0;

            dto = co_await databaseService.CreateAccount(account, encoded, gmLevel);
        }

        if (!dto.has_value() || dto->banned || dto->deleted ||
            !co_await safeHashService.Compare(dto->password, password))
        {
            client.Send(ServerType::Login, LoginPacketS2CCreator::CreateLoginFail(LoginResult::InvalidIDPassword));

            co_return;
        }

        AuthenticationService& authenticationService = _serviceLocator.Get<AuthenticationService>();

        std::shared_ptr<AuthenticationToken> token = co_await authenticationService.Add(dto->id, client.GetId());
        if (!token)
        {
            token = co_await authenticationService.Extract(dto->id);
            if (token)
            {
                if (const std::shared_ptr<GameClient> releaseTarget = _serviceLocator.Get<GameClientStorage>().Extract(token->GetClientId());
                    releaseTarget)
                {
                    releaseTarget->Disconnect();
                }
            }

            client.Send(ServerType::Login, LoginPacketS2CCreator::CreateLoginFail(LoginResult::ReleaseExistingConnection));

            co_return;
        }

        const auto& lobbyList = co_await _serviceLocator.Get<GatewayService>().GetLobbies();

        client.SetState(GameClientState::LoginAuthenticated);
        client.SetAuthenticationToken(std::move(token));

        client.Send(ServerType::Login, LoginPacketS2CCreator::CreateServerList(lobbyList));
    }

    auto LoginPacketC2SHandler::HandleWorldSelect(GameClient& client, BufferReader& reader) const -> Future<void>
    {
        int32_t key1 = reader.Read<int32_t>();
        int32_t key2 = reader.Read<int32_t>();
        const int8_t sid = reader.Read<int8_t>();

        const std::shared_ptr<AuthenticationToken>& token = client.GetAuthenticationToken();
        token->SetSid(sid);

        // replace use client key to server auth token
        const AuthenticationToken::Key& authTokenKey = token->GetKey();

        key1 = authTokenKey.GetLow();
        key2 = authTokenKey.GetHigh();

        client.SetState(GameClientState::LoginToLobby);

        client.Send(ServerType::Login, LoginPacketS2CCreator::CreateLobbyPass(key1, key2, sid));

        co_return;
    }

    auto LoginPacketC2SHandler::ReadString(BufferReader& reader, int64_t bufferSize) -> std::string
    {
        std::vector<char> buffer(bufferSize);

        reader.ReadBuffer(buffer.data(), bufferSize);

        std::string result(buffer.begin(), buffer.end());

        while (!result.empty() && result.back() == '\0')
        {
            result.pop_back();
        }

        return result;
    }
}

#pragma once
#include "sl/generator/server/client/game_client_state.h"
#include "sl/generator/server/packet/login_packet_c2s.h"
#include "sl/generator/server/packet/handler/packet_c2s_handler_interface.h"

namespace sunlight
{
    class GameClient;
}

namespace sunlight
{
    class LoginPacketC2SHandler final
        : public IPacketC2SHandler
        , public std::enable_shared_from_this<LoginPacketC2SHandler>
    {
    public:
        explicit LoginPacketC2SHandler(const ServiceLocator& serviceLocator);

        auto HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void> override;

        auto GetName() const -> std::string_view;

    private:
        void BindHandler(GameClientState state, LoginPacketC2S opcode,
            Future<void>(LoginPacketC2SHandler::* handler)(GameClient&, BufferReader&) const);

        auto HandleLogin(GameClient& client, BufferReader& reader) const -> Future<void>;
        auto HandleWorldSelect(GameClient& client, BufferReader& reader) const -> Future<void>;

    private:
        static auto ReadString(BufferReader& reader, int64_t bufferSize) -> std::string;

    private:
        const ServiceLocator& _serviceLocator;

        std::unordered_map<GameClientState,
            std::unordered_map<LoginPacketC2S, std::function<Future<void>(GameClient&, BufferReader&)>>> _handlers;
    };
}

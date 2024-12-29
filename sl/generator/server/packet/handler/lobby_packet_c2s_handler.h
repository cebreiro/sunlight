#pragma once
#include "sl/generator/server/client/game_client_state.h"
#include "sl/generator/server/packet/lobby_packet_c2s.h"
#include "sl/generator/server/packet/handler/packet_c2s_handler_interface.h"

namespace sunlight
{
    class LobbyServer;
    class SlPacketReader;
}

namespace sunlight
{
    class LobbyPacketC2SHandler final
        : public IPacketC2SHandler
        , public std::enable_shared_from_this<LobbyPacketC2SHandler>
    {
    public:
        LobbyPacketC2SHandler(const ServiceLocator& serviceLocator, LobbyServer& lobbyServer);

        auto HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void> override;

        auto GetName() const -> std::string_view;

    private:
        void Bind(GameClientState state, LobbyPacketC2S opcode,
            Future<void>(LobbyPacketC2SHandler::* handler)(ServerConnection&, SlPacketReader&) const);

        auto HandleVersionCheck(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;
        auto HandleAuthentication(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;

        auto HandleCharacterList(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;
        auto HandleCharacterNameCheck(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;
        auto HandleCharacterCreate(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;
        auto HandleCharacterDelete(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;
        auto HandleCharacterSelect(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;

    private:
        const ServiceLocator& _serviceLocator;
        LobbyServer& _lobbyServer;

        std::unordered_map<LobbyPacketC2S, std::function<Future<void>(ServerConnection&, SlPacketReader&)>> _handlers;
        std::unordered_multimap<GameClientState, LobbyPacketC2S> _allows;
    };
}

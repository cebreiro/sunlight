#pragma once
#include "sl/emulator/server/client/game_client_state.h"
#include "sl/emulator/server/packet/zone_packet_c2s.h"
#include "sl/emulator/server/packet/handler/packet_c2s_handler_interface.h"

namespace sunlight
{
    class ZoneServer;
    class SlPacketReader;
}

namespace sunlight
{
    class ZonePacketC2SHandler final
        : public IPacketC2SHandler
        , public std::enable_shared_from_this<ZonePacketC2SHandler>
    {
    public:
        ZonePacketC2SHandler(const ServiceLocator& serviceLocator, ZoneServer& zoneServer);

        auto HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void> override;

        auto GetName() const -> const std::string&;

    private:
        void Bind(GameClientState state, ZonePacketC2S opcode,
            Future<void>(ZonePacketC2SHandler::* handler)(ServerConnection&, SlPacketReader&) const);

        auto HandleLogin(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;

    private:
        const ServiceLocator& _serviceLocator;
        ZoneServer& _zoneServer;

        std::string _name;

        std::unordered_map<ZonePacketC2S, std::function<Future<void>(ServerConnection&, SlPacketReader&)>> _handlers;
        std::unordered_multimap<GameClientState, ZonePacketC2S> _allows;
    };
}

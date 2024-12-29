#pragma once
#include "sl/generator/server/client/game_client_state.h"
#include "sl/generator/server/packet/zone_packet_c2s.h"
#include "sl/generator/server/packet/handler/packet_c2s_handler_interface.h"

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
        auto HandleLogin(ServerConnection& connection, SlPacketReader& reader) const -> Future<void>;
        void Delegate(ServerConnection& connection, ZonePacketC2S opcode, UniquePtrNotNull<SlPacketReader> reader) const;

    private:
        const ServiceLocator& _serviceLocator;
        ZoneServer& _zoneServer;

        std::string _name;
    };
}

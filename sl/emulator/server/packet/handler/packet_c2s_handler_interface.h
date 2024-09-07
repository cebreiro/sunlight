#pragma once

namespace sunlight
{
    class ServerConnection;
}

namespace sunlight
{
    class IPacketC2SHandler
    {
    public:
        virtual ~IPacketC2SHandler() = default;

        virtual auto HandlePacket(ServerConnection& connection, Buffer packet) const -> Future<void> = 0;
    };
}

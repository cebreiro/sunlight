#include "lobby_s2c_endpoint.h"

namespace sunlight
{
    LobbyS2CEndpoint::LobbyS2CEndpoint(const std::string& address, uint16_t port)
        : address(address)
        , port(port)
    {
    }

    auto LobbyS2CEndpoint::Serialize() const -> Buffer
    {
        PacketWriter writer;

        writer.Write<int32_t>(port);
        writer.WriteString(address);

        return writer.Flush();
    }
}

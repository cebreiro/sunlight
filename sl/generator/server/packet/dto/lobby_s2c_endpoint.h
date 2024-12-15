#pragma once

namespace sunlight
{
    struct LobbyS2CEndpoint
    {
        LobbyS2CEndpoint() = default;
        LobbyS2CEndpoint(const std::string& address, uint16_t port);

        std::string address;
        int32_t port = 0;

        auto Serialize() const -> Buffer;
    };
}

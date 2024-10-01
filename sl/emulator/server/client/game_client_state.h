#pragma once

namespace sunlight
{
    ENUM_CLASS(GameClientState, int32_t,
        (None)
        (LoginConnected)
        (LoginAuthenticated)
        (LoginToLobby)
        (LobbyAuthenticated)
        (LobbyAndZoneConnecting)
        (LobbyAndZoneAuthenticated)
        (ZoneChaning)
    )
}

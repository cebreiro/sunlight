#pragma once

namespace sunlight
{
    ENUM_CLASS(LoginPacketS2C, int8_t,
        (Hello, 0)
        (LoginFail, 1)
        (WorldList, 4)
        (LobbyPass, 7)
    )
}

#pragma once

namespace sunlight
{
    ENUM_CLASS(LoginPacketC2S, int8_t,
        (Login, int8_t{ 0 })
        (RequestBackToLoginServer, int8_t{ 1 })
        (SelectWorld, int8_t{ 2 })
        (CloseSessionRequest, int8_t{ 3 })
        (UnknownOpcode04, int8_t{ 4 })
        (UnknownOpcode05, int8_t{ 5 })
        (ReLogin, int8_t{ 6 })
        (UnknownOpcode07, int8_t{ 7 })
    )
}

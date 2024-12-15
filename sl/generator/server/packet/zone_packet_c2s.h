#pragma once

namespace sunlight
{
    ENUM_CLASS(ZonePacketC2S, uint8_t,
        (NMC_LOGIN, 0x50)
        (NMC_LOGOUT, 0x51)
        (NMC_REQ_MOVE, 0x60)
        (NMC_REQ_STOP, 0x61)
        (NMC_REQ_SETDIRECTION, 0x62)
        (NMC_REQ_CHANGE_ROOM, 0x63)
        (NMC_REQ_USERINFO, 0x64)
        (NMC_REQ_UNLOCK, 0x65)
        (NMC_TRIGGER_EVENT, 0x90) // 0x591B3B
        (NMC_SEND_MESSAGE_TO, 0x91) // 0x591BE1
        (NMC_SEND_LOCAL_MESSAGE, 0x92) // 0x591CEB
        (NMC_SEND_GLOBAL_MESSAGE, 0x93) // 0x591C5B
        (UNKNOWN_0x86, 0x86) // 0x44A47F
    )
}

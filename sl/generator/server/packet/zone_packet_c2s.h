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
        (NMC_TRIGGER_EVENT, 0x90)
        (NMC_SEND_MESSAGE_TO, 0x91)
        (NMC_SEND_LOCAL_MESSAGE, 0x92)
        (NMC_SEND_GLOBAL_MESSAGE, 0x93)
        (UNKNOWN_0x86, 0x86)
    )
}

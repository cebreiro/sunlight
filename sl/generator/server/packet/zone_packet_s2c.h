#pragma once

namespace sunlight
{
    ENUM_CLASS(ZonePacketS2C, uint8_t,
        (NMS_LOGIN_ACCEPT, 0x10)
        (NMS_LOGIN_REJECT, 0x11)
        (NMS_DISCONNECT, 0x12)
        (NMS_TCP_NODELAY_CHANGE, 0x13)
        (NMS_OBJECT_LEAVE, 0x21)
        (NMS_OBJECT_MOVE, 0x22)
        (UNKNOWN_0x23, 0x23)
        (NMS_MOVE, 0x24)
        (NMS_FORCE_MOVE, 0x30)
        (NMS_USERINFO, 0x31)
        (NMS_ROOM_CHANGE, 0x32)
        (NMS_VISIBLE_RANGE, 0x33)
        (NMS_DELIVER_MESSAGE, 0x40)
    )
}

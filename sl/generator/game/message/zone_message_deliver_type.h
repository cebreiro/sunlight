#pragma once

namespace sunlight
{
    ENUM_CLASS(ZoneMessageDeliverType, uint8_t,
        (MSG_SC_CHANGE_CLIENT_ZONE_TO, 4)
        (MSG_SC_NORMAL_MESSAGE, 5)
        (MSG_SC_GOB_MESSAGE, 6)
        (MSG_SC_GOB_DATA, 7)
        (MSG_SC_CHR_MESSAGE, 8)
        (MSG_SC_NOTICE_MESSAGE, 9)
        (MSG_SC_SHOUT_MESSAGE, 10)
        (UNK11, 11)
        (MSG_SC_CHR_MESSAGE_RESULT, 12)
    )
}

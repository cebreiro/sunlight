#pragma once

namespace sunlight
{
    // client 0x4C5A30
    ENUM_CLASS(ChannelNotifyType, int32_t,
        (None, 0)
        (Leave, 1)
        (Enter, 2)
        (ForceLeave, 4)
        (PartyPlayerUpdate, 5)
        (PartyOptionChange, 6)
        (PartyDisband, 7)
        (PartyLeaderChange, 8)
        (PartyAutoDisband, 9)
    )
}

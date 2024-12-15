#pragma once

namespace sunlight
{
    ENUM_CLASS(CharacterMessageType, int32_t,
        (Whisper, 0xA)
        (WhereAreYou, 0x14)
        (IamHere, 0x15)
        (ShowYou, 0x19)
        (ShowYouAck, 0x1A)
        (AckMe, 0x1F)
        (AckYou, 0x20)
        (JoinReq, 0x28)
        (JoinAck, 0x29)
        (JoinReje, 0x2A)
        (ChannelMsg, 0x32)
        (ChannelNotifyMsg, 0x33)
        (ChannelInvite, 0x36)
        (ChannelInviteResult, 0x37)
        (ParallelRoomHandle, 0x38)
        (ParallelRoomHandleAck, 0x39)
    )
}

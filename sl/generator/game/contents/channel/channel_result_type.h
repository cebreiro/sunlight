#pragma once

namespace sunlight
{
    ENUM_CLASS(ChannelInviteResult, int8_t,
        (AlreadyHasParty, 0)
        (RefuseParty, 1)
        (AcceptPartyCreation, 2)
        (AcceptPartyJoin, 3)
        (RefuseChannel1, 4)
        (RefuseChannel2, 6)
    )

    ENUM_CLASS(ChannelJoinResult, int32_t,
        (Success, 0)
        (Fail, 0x10000)
        (Fail2, 0x30004)
        (Fail_IsFull, 0x30005)
        (PasswordError, 0x30006)
        (NotExist, 0x30007)
    )
}

#pragma once

namespace sunlight
{
    ENUM_CLASS(PlayerProfileSetting, int8_t,
        (RefusePartyInvite, 1)
        (RefuseChannelInvite, 2)
        (RefuseGuildInvite, 4)
        (Private, 8)
    )
}

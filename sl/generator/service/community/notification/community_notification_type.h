#pragma once
#include <boost/describe.hpp>

namespace sunlight
{
    ENUM_CLASS(CommunityNotificationType, int32_t,
        (PlayerKick)
        (PartyCreateResult)
        (PartyList)
        (PartyInvite)
        (PartyInviteRefused)
        (PartyJoinResult)
        (PartyLeave)
        (PartyMemberAdd)
        (PartyDisband)
        (PartyForceExit)
        (PartyLeaderChange)
        (PartyOptionChange)
        (PartyJoinRequest)
        (PartyJoinRejected)
        (PartyChat)
        (GlobalChat)
        (WhisperChat)
        (WhisperChatFail)
    )

    BOOST_DESCRIBE_ENUM(CommunityNotificationType,
        PlayerKick,
        PartyCreateResult,
        PartyList,
        PartyInvite,
        PartyInviteRefused,
        PartyJoinResult,
        PartyLeave,
        PartyMemberAdd,
        PartyDisband,
        PartyForceExit,
        PartyLeaderChange,
        PartyOptionChange,
        PartyJoinRequest,
        PartyJoinRejected,
        PartyChat,
        GlobalChat,
        WhisperChat,
        WhisperChatFail
    )

    template <CommunityNotificationType E>
    struct community_notification_type_mapping;
}

#define SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(className)\
template <>\
struct community_notification_type_mapping<className::TYPE>\
{\
    using type = className;\
}

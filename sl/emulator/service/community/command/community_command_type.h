#pragma once
#include <boost/describe.hpp>

namespace sunlight
{
    ENUM_CLASS(CommunityCommandType, int32_t,
        (PlayerRegister)
        (PlayerDeregister)
        (PlayerDeregisterTimer)
        (PartyCreate)
        (PartyInviteResult)
    )

    BOOST_DESCRIBE_ENUM(CommunityCommandType,
        PlayerRegister,
        PlayerDeregister,
        PartyCreate,
        PartyInviteResult
    )

    template <CommunityCommandType E>
    struct community_command_type_mapping;
}

#define SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(className)\
template <>\
struct community_command_type_mapping<className::TYPE>\
{\
    using type = className;\
}

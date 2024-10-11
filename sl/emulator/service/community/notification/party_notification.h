#pragma once
#include "sl/emulator/game/contents/channel/channel_result_type.h"
#include "sl/emulator/service/community/notification/community_notification_interface.h"
#include "sl/emulator/service/community/party/party_information.h"
#include "sl/emulator/service/community/party/party_player_information.h"

namespace sunlight
{
    struct PartyNotificationCreateResult : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyCreateResult;

        int64_t playerId = 0;

        PartyPlayerInformation leader;
        PartyPlayerInformation member;
        PartyInformation information;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationCreateResult);

    struct PartyNotificationPartyInvite : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyInvite;

        bool creation = false;
        std::string partyName;
        std::string inviterName;
        int64_t playerId = 0;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyInvite);

    struct PartyNotificationPartyInviteRefused : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyInviteRefused;

        int64_t playerId = 0;
        std::string refuserName;
        ChannelInviteResult result = {};

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyInviteRefused);
}

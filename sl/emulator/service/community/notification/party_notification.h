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

    struct PartyNotificationPartyJoinResult : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyJoinResult;

        int64_t playerId = 0;
        std::string partyName;
        ChannelJoinResult result = {};

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyJoinResult);

    struct PartyNotificationPartyLeave : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyLeave;

        int64_t playerId = 0;
        std::string partyName;
        std::string leaverName;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyLeave);

    struct PartyNotificationPartyMemberAdd : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyMemberAdd;

        int64_t playerId = 0;
        std::string partyName;
        std::vector<PartyPlayerInformation> members;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyMemberAdd);

    struct PartyNotificationPartyDisband : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyDisband;

        int64_t playerId = 0;
        std::string partyName;
        bool autoDisband = false;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyDisband);

    struct PartyNotificationPartyPlayerStateRequested : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyPlayerStateRequested;

        int64_t playerId = 0;
        int64_t requesterId = 0;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyPlayerStateRequested);

    struct PartyNotificationPartyPlayerState : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyPlayerState;

        int64_t playerId = 0;
        std::string targetName;

        float x = 0.f;
        float y = 0.f;
        float hp = 0.f;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyPlayerState);
}

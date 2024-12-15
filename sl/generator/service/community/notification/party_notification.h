#pragma once
#include "sl/generator/game/contents/channel/channel_result_type.h"
#include "sl/generator/service/community/notification/community_notification_interface.h"
#include "sl/generator/service/community/party/party_information.h"
#include "sl/generator/service/community/party/party_player_information.h"

namespace sunlight
{
    struct PartyNotificationCreateResult : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyCreateResult;

        int64_t playerId = 0;

        PartyPlayerInformation leader;
        PartyPlayerInformation member;
        PartyInformation party;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationCreateResult);

    struct PartyNotificationPartyList : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyList;

        int64_t playerId = 0;

        std::vector<PartyInformation> parties;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyList);

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

        std::vector<PartyPlayerInformation> players;
        std::optional<PartyInformation> information;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyJoinResult);

    struct PartyNotificationPartyLeave : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyLeave;

        int64_t playerId = 0;
        std::string partyName;
        std::string leaverName;
        int64_t leaverId = 0;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyLeave);

    struct PartyNotificationPartyMemberAdd : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyMemberAdd;

        int64_t playerId = 0;
        std::string partyName;
        PartyPlayerInformation member;

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

    struct PartyNotificationPartyForceExit : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyForceExit;

        int64_t playerId = 0;
        std::string partyName;
        std::string targetName;
        int64_t targetId = 0;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyForceExit);

    struct PartyNotificationPartyLeaderChange : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyLeaderChange;

        int64_t playerId = 0;
        std::string partyName;
        std::string newLeaderName;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyLeaderChange);

    struct PartyNotificationPartyOptionChange : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyOptionChange;

        int64_t playerId = 0;

        PartyInformation party;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyOptionChange);

    struct PartyNotificationPartyJoinRequest : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyJoinRequest;

        int64_t playerId = 0;
        std::string requesterName;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyJoinRequest);

    struct PartyNotificationPartyJoinRejected : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyJoinRejected;

        int64_t playerId = 0;
        std::string partyLeaderName;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyJoinRejected);

    struct PartyNotificationPartyChat : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PartyChat;

        int64_t playerId = 0;
        std::string sender;
        std::string message;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(PartyNotificationPartyChat);
}

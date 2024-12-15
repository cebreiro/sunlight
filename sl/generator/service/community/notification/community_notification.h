#pragma once
#include "sl/generator/service/community/command/community_command.h"
#include "sl/generator/service/community/notification/community_notification_interface.h"

namespace sunlight
{
    struct CommunityNotificationPlayerKick : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PlayerKick;

        int64_t playerId = 0;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(CommunityNotificationPlayerKick);

    struct CommunityNotificationGlobalChat : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::GlobalChat;

        CommunityChatType type = {};
        std::string sender;
        std::string message;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(CommunityNotificationGlobalChat);

    struct CommunityNotificationWhisperChat : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::WhisperChat;

        int64_t playerId = 0;
        std::string sender;
        std::string message;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(CommunityNotificationWhisperChat);

    struct CommunityNotificationWhisperChatFail : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::WhisperChatFail;

        int64_t playerId = 0;
        std::string whisperTarget;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(CommunityNotificationWhisperChatFail);
}

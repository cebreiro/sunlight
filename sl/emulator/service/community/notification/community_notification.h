#pragma once
#include "sl/emulator/service/community/notification/community_notification_interface.h"

namespace sunlight
{
    struct CommunityNotificationPlayerKick : ICommunityNotification
    {
        static constexpr auto TYPE = CommunityNotificationType::PlayerKick;

        int64_t playerId = 0;

        auto GetType() const -> CommunityNotificationType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_NOTIFICATION_MAPPER(CommunityNotificationPlayerKick);
}

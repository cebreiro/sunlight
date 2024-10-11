#pragma once
#include "sl/emulator/service/community/command/community_command_interface.h"

namespace sunlight
{
    struct CommunityCommandPlayerRegister : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PlayerRegister;

        int32_t zoneId = 0;
        int64_t playerId = 0;
        std::string name;

        int32_t jobId = 0;
        int8_t jobLevel = 0;
        int8_t characterLevel = 0;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(CommunityCommandPlayerRegister);

    struct CommunityCommandPlayerDeregister : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PlayerDeregister;

        int32_t zoneId = 0;
        int64_t playerId = 0;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(CommunityCommandPlayerDeregister);

    struct CommunityCommandPlayerDeregisterTimer : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PlayerDeregisterTimer;

        int32_t zoneId = 0;
        int64_t playerId = 0;
        int64_t milliseconds = 0;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(CommunityCommandPlayerDeregisterTimer);
}

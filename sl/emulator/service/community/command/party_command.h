#pragma once
#include "sl/emulator/game/contents/channel/channel_result_type.h"
#include "sl/emulator/service/community/command/community_command_interface.h"

namespace sunlight
{
    struct PartyCommandCreate : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyCreate;

        int64_t playerId = 0;
        std::string partyName;
        std::string targetName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandCreate);

    struct PartyCommandInviteResult : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyInviteResult;

        int64_t playerId = 0;
        std::string inviterName;
        ChannelInviteResult result = {};

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandInviteResult);
}

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

    struct PartyCommandInvite : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyInvite;

        int64_t playerId = 0;
        std::string targetName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandInvite);

    struct PartyCommandInviteResult : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyInviteResult;

        int64_t playerId = 0;
        std::string inviterName;
        ChannelInviteResult result = {};

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandInviteResult);

    struct PartyCommandPartyPlayerStateRequest : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyPlayerStateRequest;

        int64_t playerId = 0;
        std::string targetName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyPlayerStateRequest);

    struct PartyCommandPartyPlayerStateResponse : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyPlayerStateResponse;

        int64_t playerId = 0;
        int64_t requestId = 0;
        float x = 0.f;
        float y = 0.f;
        float hp = 0.f;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyPlayerStateResponse);
}

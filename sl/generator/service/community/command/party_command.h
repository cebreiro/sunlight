#pragma once
#include "sl/generator/game/contents/channel/channel_result_type.h"
#include "sl/generator/service/community/command/community_command_interface.h"

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

    struct PartyCommandPartyList : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyList;

        int64_t playerId = 0;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyList);

    struct PartyCommandPartyLeave : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyLeave;

        int64_t playerId = 0;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyLeave);

    struct PartyCommandPartyForceExit : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyForceExit;

        int64_t playerId = 0;
        std::string targetName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyForceExit);

    struct PartyCommandPartyLeaderChange : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyLeaderChange;

        int64_t playerId = 0;
        std::string newLeaderName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyLeaderChange);

    struct PartyCommandPartyOptionChange : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyOptionChange;

        int64_t playerId = 0;

        bool isPublic = false;
        bool setGoldDistribution = false;
        bool setItemDistribution = false;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyOptionChange);

    struct PartyCommandPartyJoin : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyJoin;

        int64_t playerId = 0;
        std::string targetName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyJoin);

    struct PartyCommandPartyJoinAck : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyJoinAck;

        int64_t playerId = 0;
        std::string targetName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyJoinAck);

    struct PartyCommandPartyJoinReject : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PartyJoinReject;

        int64_t playerId = 0;
        std::string targetName;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(PartyCommandPartyJoinReject);
}

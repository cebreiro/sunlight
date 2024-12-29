#pragma once
#include "sl/generator/service/community/community_chat_type.h"
#include "sl/generator/service/community/command/community_command_interface.h"

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
        int32_t maxHP = 0;

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

    struct CommunityCommandPlayerUpdateInformation : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::PlayerUpdateInformation;

        int32_t zoneId = 0;
        int64_t playerId = 0;
        int32_t jobId = 0;
        int8_t jobLevel = 0;
        int8_t characterLevel = 0;
        int32_t maxHP = 0;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(CommunityCommandPlayerUpdateInformation);

    struct CommunityCommandChatDeliver : ICommunityCommand
    {
        static constexpr auto TYPE = CommunityCommandType::ChatDeliver;

        CommunityChatType type = {};
        int64_t requesterId = 0;
        std::string message;

        std::optional<std::string> target = std::nullopt;

        auto GetType() const -> CommunityCommandType override { return TYPE; }
    };
    SUNLIGHT_DEFINE_COMMUNITY_COMMAND_MAPPER(CommunityCommandChatDeliver);
}

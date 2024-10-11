#pragma once
#include "sl/emulator/game/contents/channel/channel_result_type.h"
#include "sl/emulator/game/message/character_message_type.h"

namespace sunlight::sox
{
    struct Zone;
}

namespace sunlight
{
    struct PartyInformation;
    struct PartyPlayerInformation;

    class GamePlayer;
}

namespace sunlight
{
    class CharacterMessageCreator
    {
    public:
        CharacterMessageCreator() = delete;

        static auto CreateProfile(const GamePlayer& targetPlayer, const sox::Zone& zone) -> Buffer;
        static auto CreateProfileFail(const std::string& targetPlayerName) -> Buffer;

        static auto CreateMessageResult(const std::string& targetPlayerName, CharacterMessageType type, bool success) -> Buffer;

        static auto CreatePartyInvite(const std::string& inviter, const std::string& partyName, const std::string& unk1, int8_t creation) -> Buffer;
        static auto CreatePartyInviteResult(const std::string& inviter, ChannelInviteResult result) -> Buffer;

        static auto CreatePartyCreate(const PartyInformation& party, const PartyPlayerInformation& leader, const PartyPlayerInformation& member) -> Buffer;
    };
}

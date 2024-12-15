#pragma once
#include "sl/generator/game/contents/channel/channel_result_type.h"
#include "sl/generator/game/message/character_message_type.h"
#include "sl/generator/service/community/party/party_player_information.h"

namespace sunlight::sox
{
    struct Zone;
}

namespace sunlight
{
    struct PartyInformation;

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

        static auto CreateIamHere(const std::string& targetPlayerName, int8_t type, float x, float y, float hp) -> Buffer;
        static auto CreateMessageResult(const std::string& targetPlayerName, CharacterMessageType type, bool success) -> Buffer;
        static auto CreateJoinResult(const std::string& partyName, ChannelJoinResult result) -> Buffer;

        static auto CreatePartyInvite(const std::string& inviter, const std::string& partyName, const std::string& unk1, int8_t creation) -> Buffer;
        static auto CreatePartyInviteResult(const std::string& inviter, ChannelInviteResult result) -> Buffer;

        static auto CreatePartyCreate(const PartyInformation& party, const PartyPlayerInformation& leader, const PartyPlayerInformation& member) -> Buffer;
        static auto CreatePartyList(std::span<const PartyInformation> parties) -> Buffer;
        static auto CreatePartyQueryResult(const std::string& partyName, std::span<const PartyPlayerInformation> members) -> Buffer;

        static auto CreatePartyMemberAdd(const std::string& partyName, const PartyPlayerInformation& member) -> Buffer;
        static auto CreatePartyMemberEnterNotify(const std::string& partyName, const std::string& target) -> Buffer;
        static auto CreatePartyLeave() -> Buffer;
        static auto CreatePartyMemberLeave(const std::string& partyName, const std::string& memberName) -> Buffer;
        static auto CreatePartyForceExit(const std::string& partyName, const std::string& targetName) -> Buffer;
        static auto CreatePartyDisband(const std::string& partyName, bool autoDisband) -> Buffer;
        static auto CreatePartyLeaderChange(const std::string& partyName, const std::string& newLeaderName) -> Buffer;
        static auto CreatePartyOptionChange(const PartyInformation& party) -> Buffer;
        static auto CreatePartyJoinRequest(const std::string& requesterName) -> Buffer;
        static auto CreatePartyJoinRejected(const std::string& partyLeaderName) -> Buffer;

    private:
        static auto WritePartyPlayer(const PartyPlayerInformation& player) -> Buffer;
        static auto WriteParty(const PartyInformation& party) -> Buffer;
    };
}

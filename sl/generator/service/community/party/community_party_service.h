#pragma once
#include "sl/generator/service/community/command/party_command.h"
#include "sl/generator/service/community/party/party.h"
#include "sl/generator/service/community/party/party_information.h"
#include "sl/generator/service/community/party/party_player_information.h"
#include "sl/generator/service/community/party/pending_party.h"
#include "sl/generator/service/community/player/community_player.h"

namespace sunlight
{
    class CommunityPlayer;
    class CommunityService;
}

namespace sunlight
{
    class CommunityPartyService final : public IService
    {
    public:
        explicit CommunityPartyService(CommunityService& communityService);

        auto GetName() const -> std::string_view override;

    public:
        void OnPlayerExit(const CommunityPlayer& player);
        void OnPlayerReconnect(const CommunityPlayer& player);

    public:
        void HandleCommand(const PartyCommandCreate& command);
        void HandleCommand(const PartyCommandInvite& command);
        void HandleCommand(const PartyCommandInviteResult& command);
        void HandleCommand(const PartyCommandPartyList& command);
        void HandleCommand(const PartyCommandPartyLeave& command);
        void HandleCommand(const PartyCommandPartyForceExit& command);
        void HandleCommand(const PartyCommandPartyLeaderChange& command);
        void HandleCommand(const PartyCommandPartyOptionChange& command);
        void HandleCommand(const PartyCommandPartyJoin& command);
        void HandleCommand(const PartyCommandPartyJoinAck& command);
        void HandleCommand(const PartyCommandPartyJoinReject& command);

        void BroadcastPartyChatting(int64_t partyId, const std::string& sender, const std::string& message);

    private:
        void ProcessPartyMemberAdd(Party& party, CommunityPlayer& newMember);
        void ProcessDisbandment(Party& party, bool isAutoDisbandment);

        void Visit(const Party& party, const std::function<void(CommunityPlayer&)>& visitor);

        auto FindParty(int64_t partyId) -> Party*;
        auto FindParty(int64_t partyId) const -> const Party*;

    private:
        void SelectPartyForListing(std::vector<PtrNotNull<const Party>>& result);

        auto CreatePartyInformation(const Party& party) const -> PartyInformation;
        auto CreatePartyPlayerInformation(const CommunityPlayer& player) const -> PartyPlayerInformation;
        auto CreatePartyPlayerInformationList(const Party& party) const -> std::vector<PartyPlayerInformation>;

    private:
        CommunityService& _communityService;
        const ServiceLocator& _serviceLocator;

        int64_t _nextPartyId = 1;
        std::unordered_map<std::string, PendingParty> _pendingParties;
        std::unordered_map<int64_t, Party> _parties;

        std::vector<PtrNotNull<const Party>> _partyListingBuffer;
        static constexpr std::chrono::seconds max_party_pending_duration = std::chrono::seconds(30);
    };
}

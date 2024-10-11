#pragma once
#include "sl/emulator/service/community/command/party_command.h"
#include "sl/emulator/service/community/party/party.h"
#include "sl/emulator/service/community/party/party_information.h"
#include "sl/emulator/service/community/party/party_player_information.h"
#include "sl/emulator/service/community/party/pending_party.h"

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

    public:
        void HandleCommand(const PartyCommandCreate& command);
        void HandleCommand(const PartyCommandInviteResult& command);
        void HandleCommand(const PartyCommandPartyPlayerStateRequest& command);
        void HandleCommand(const PartyCommandPartyPlayerStateResponse& command);

    private:
        auto CreatePartyInformation(const Party& party) const -> PartyInformation;
        auto CreatePartyPlayerInformation(const CommunityPlayer& player) const -> PartyPlayerInformation;

    private:
        CommunityService& _communityService;
        const ServiceLocator& _serviceLocator;

        int64_t _nextPartyId = 1;
        std::unordered_map<std::string, PendingParty> _pendingParties;
        std::unordered_map<int64_t, Party> _parties;

        static constexpr std::chrono::seconds max_party_pending_duration = std::chrono::seconds(30);
    };
}

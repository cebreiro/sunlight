#include "community_party_service.h"

#include "sl/emulator/service/community/community_service.h"
#include "sl/emulator/service/community/notification/party_notification.h"
#include "sl/emulator/service/community/player/community_player_storage.h"

namespace sunlight
{
    CommunityPartyService::CommunityPartyService(CommunityService& communityService)
        : _communityService(communityService)
        , _serviceLocator(communityService.GetServiceLocator())
    {
    }

    auto CommunityPartyService::GetName() const -> std::string_view
    {
        return "party_community_service";
    }

    void CommunityPartyService::OnPlayerExit(const CommunityPlayer& player)
    {
        (void)player;
    }

    void CommunityPartyService::HandleCommand(const PartyCommandCreate& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        CommunityPlayer* player = playerStorage.Find(command.playerId);
        if (!player)
        {
            return;
        }

        _pendingParties.erase(player->GetName());

        ChannelInviteResult result = ChannelInviteResult::RefuseParty;

        do
        {
            if (player->HasParty())
            {
                result = ChannelInviteResult::AlreadyHasParty;

                break;
            }

            CommunityPlayer* target = playerStorage.FindByName(command.targetName);
            if (!target)
            {
                result = ChannelInviteResult::RefuseParty;

                break;
            }

            auto inviteNotification = std::make_shared<PartyNotificationPartyInvite>();
            inviteNotification->creation = true;
            inviteNotification->partyName = command.partyName;
            inviteNotification->inviterName = player->GetName();
            inviteNotification->playerId = target->GetId();

            _communityService.Notify(target->GetZoneId(), std::move(inviteNotification));

            const int64_t partyId = _nextPartyId++;

            _pendingParties.try_emplace(player->GetName(), PendingParty{
                .id = partyId,
                .leaderId = player->GetId(),
                .partyName = command.partyName,
                .invited = target->GetName(),
                });

            Delay(max_party_pending_duration).Then(_communityService.GetStrand(),
                [weak = _communityService.weak_from_this(), this, partyId, playerName = player->GetName()]()
                    {
                        if (const auto instance = weak.lock(); !instance)
                        {
                            return;
                        }

                        const auto iter = _pendingParties.find(playerName);
                        if (iter == _pendingParties.end() || iter->second.id != partyId)
                        {
                            return;
                        }

                        _pendingParties.erase(iter);
                    });

            return;
            
        } while (false);

        auto refuseNotification = std::make_shared<PartyNotificationPartyInviteRefused>();

        refuseNotification->result = result;
        refuseNotification->playerId = command.playerId;
        refuseNotification->refuserName = command.targetName;

        _communityService.Notify(player->GetZoneId(), std::move(refuseNotification));
    }

    void CommunityPartyService::HandleCommand(const PartyCommandInviteResult& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        CommunityPlayer* invited = playerStorage.Find(command.playerId);
        CommunityPlayer* inviter = playerStorage.FindByName(command.inviterName);

        if (!invited || !inviter)
        {
            return;
        }

        switch (command.result)
        {
        case ChannelInviteResult::AlreadyHasParty:
        case ChannelInviteResult::RefuseParty:
        case ChannelInviteResult::RefuseChannel1:
        case ChannelInviteResult::RefuseChannel2:
        {
            auto notification = std::make_shared<PartyNotificationPartyInviteRefused>();
            notification->playerId = inviter->GetId();
            notification->result = command.result;
            notification->refuserName = invited->GetName();

            _communityService.Notify(inviter->GetZoneId(), std::move(notification));

            if (const auto iter = _pendingParties.find(command.inviterName);
                iter != _pendingParties.end())
            {
                if (iter->second.invited == invited->GetName())
                {
                    _pendingParties.erase(iter);
                }
            }

            return;
        }
        case ChannelInviteResult::AcceptPartyCreation:
        {
            if (inviter->HasParty())
            {
                return;
            }

            const auto iter = _pendingParties.find(command.inviterName);
            if (iter == _pendingParties.end())
            {
                return;
            }

            if (iter->second.leaderId != inviter->GetId())
            {
                return;
            }

            if (iter->second.invited != invited->GetName())
            {
                return;
            }

            const PendingParty& pendingParty = iter->second;
            const int64_t partyId = pendingParty.id;

            const auto [iter2, inserted] = _parties.try_emplace(partyId,
                Party(partyId, pendingParty.partyName, inviter->GetId(), invited->GetId()));
            assert(inserted);

            _pendingParties.erase(iter);

            inviter->SetPartyId(partyId);
            invited->SetPartyId(partyId);

            const Party& party = iter2->second;

            for (int64_t memberId : party.GetMembers())
            {
                auto notification = std::make_shared<PartyNotificationCreateResult>();
                notification->playerId = memberId;
                notification->leader = CreatePartyPlayerInformation(*inviter);
                notification->member = CreatePartyPlayerInformation(*invited);
                notification->information = CreatePartyInformation(party);

                _communityService.Notify(playerStorage.Find(memberId)->GetZoneId(), std::move(notification));
            }

            return;
        }
        case ChannelInviteResult::AcceptPartyJoin:
        {

            return;
        }
        }
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyPlayerStateRequest& command)
    {
        const CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* player = playerStorage.Find(command.playerId);
        const CommunityPlayer* target = playerStorage.FindByName(command.targetName);

        if (!player || !target)
        {
            return;
        }

        auto notification = std::make_shared<PartyNotificationPartyPlayerStateRequested>();
        notification->playerId = target->GetId();
        notification->requesterId = command.playerId;

        _communityService.Notify(player->GetZoneId(), std::move(notification));
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyPlayerStateResponse& command)
    {
        const CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* player = playerStorage.Find(command.playerId);
        const CommunityPlayer* requester = playerStorage.Find(command.requestId);

        if (!player || !requester)
        {
            return;
        }

        auto notification = std::make_shared<PartyNotificationPartyPlayerState>();
        notification->playerId = requester->GetId();
        notification->targetName = player->GetName();
        notification->x = command.x;
        notification->y = command.y;
        notification->hp = command.hp;

        _communityService.Notify(requester->GetZoneId(), std::move(notification));
    }

    auto CommunityPartyService::CreatePartyInformation(const Party& party) const -> PartyInformation
    {
        const CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();
        const CommunityPlayer* leader = playerStorage.Find(party.GetLeaderId());
        assert(leader);

        PartyInformation information;
        information.partyName = party.GetName();
        information.leaderName = leader ? leader->GetName() : "";
        information.leaderCharacterLevel = leader ? leader->GetCharacterLevel() : 0;
        information.leaderZoneId = leader ? leader->GetZoneId() : 0;

        information.memberCount = static_cast<int8_t>(std::ssize(party.GetMembers()));
        information.isPublic = party.IsPublic();
        information.goldDistribution = party.IsSetGoldDistribution();
        information.itemDistribution = party.IsSetItemDistribution();

        return information;
    }

    auto CommunityPartyService::CreatePartyPlayerInformation(const CommunityPlayer& player) const -> PartyPlayerInformation
    {
        return PartyPlayerInformation{
            .name = player.GetName(),
            .jobId = player.GetJobId(),
            .jobLevel = player.GetJobLevel(),
            .hp = player.GetHP(),
            .maxHP = player.GetMaxHP(),
        };
    }
}

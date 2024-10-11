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

        if (player->HasParty())
        {
            auto notification = std::make_shared<PartyNotificationPartyInviteRefused>();

            notification->result = ChannelInviteResult::AlreadyHasParty;
            notification->playerId = command.playerId;
            notification->refuserName = command.targetName;

            _communityService.Notify(player->GetZoneId(), std::move(notification));
            
            return;
        }

        CommunityPlayer* target = playerStorage.FindByName(command.targetName);
        if (!target)
        {
            return;
        }

        auto notification = std::make_shared<PartyNotificationPartyInvite>();
        notification->creation = true;
        notification->partyName = command.partyName;
        notification->inviterName = player->GetName();
        notification->playerId = target->GetId();

        _communityService.Notify(target->GetZoneId(), std::move(notification));

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
    }

    void CommunityPartyService::HandleCommand(const PartyCommandInvite& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        CommunityPlayer* player = playerStorage.Find(command.playerId);
        CommunityPlayer* target = playerStorage.FindByName(command.targetName);

        if (!player || !target)
        {
            return;
        }

        if (!player->HasParty())
        {
            return;
        }

        const Party* party = FindParty(player->GetPartyId());
        if (!party)
        {
            return;
        }

        if (target->HasParty())
        {
            auto notification = std::make_shared<PartyNotificationPartyInviteRefused>();

            notification->result = ChannelInviteResult::AlreadyHasParty;
            notification->playerId = command.playerId;
            notification->refuserName = command.targetName;

            _communityService.Notify(player->GetZoneId(), std::move(notification));

            return;
        }

        auto notification = std::make_shared<PartyNotificationPartyInvite>();
        notification->creation = false;
        notification->partyName = party->GetName();
        notification->inviterName = player->GetName();
        notification->playerId = target->GetId();

        _communityService.Notify(target->GetZoneId(), std::move(notification));
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
            ChannelJoinResult result = {};
            std::string partyName;

            do
            {
                if (!inviter->HasParty())
                {
                    result = ChannelJoinResult::NotExist;

                    break;
                }

                Party* party = FindParty(inviter->GetPartyId());
                if (!party)
                {
                    result = ChannelJoinResult::NotExist;

                    break;
                }

                partyName = party->GetName();

                if (party->Contains(invited->GetId()))
                {
                    result = ChannelJoinResult::Fail;

                    break;
                }

                if (party->IsFull())
                {
                    result = ChannelJoinResult::Fail_IsFull;

                    break;
                }

                party->Add(invited->GetId());
                invited->SetPartyId(party->GetId());

                std::vector<PartyPlayerInformation> informationList = CreatePartyPlayerInformationList(*party);

                for (const int64_t partyPlayerId : party->GetMembers())
                {
                    const CommunityPlayer* partyPlayer = playerStorage.Find(partyPlayerId);
                    if (!partyPlayerId)
                    {
                        assert(false);

                        continue;
                    }

                    auto notification = std::make_shared<PartyNotificationPartyMemberAdd>();
                    notification->playerId = partyPlayerId;
                    notification->partyName = party->GetName();
                    notification->members = informationList;

                    _communityService.Notify(partyPlayer->GetZoneId(), std::move(notification));
                }

            } while (false);

            auto joinResultNotification = std::make_shared<PartyNotificationPartyJoinResult>();
            joinResultNotification->playerId = invited->GetId();
            joinResultNotification->partyName = std::move(partyName);
            joinResultNotification->result = result;

            _communityService.Notify(invited->GetZoneId(), std::move(joinResultNotification));

            return;
        }
        }
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyLeave& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();
        const int64_t playerId = command.playerId;

        CommunityPlayer* player = playerStorage.Find(playerId);
        if (!player || !player->HasParty())
        {
            return;
        }

        Party* party = FindParty(player->GetPartyId());
        if (!party)
        {
            assert(false);

            return;
        }

        if (!party->Contains(playerId))
        {
            return;
        }

        bool shouldDisband = false;
        bool autoDisband = false;

        if (party->GetLeaderId() == playerId)
        {
            shouldDisband = true;
        }
        else
        {
            party->Remove(playerId);
            player->SetPartyId(std::nullopt);

            for (int64_t memberId : party->GetMembers())
            {
                const CommunityPlayer* member = playerStorage.Find(memberId);
                if (!member)
                {
                    assert(false);

                    continue;
                }

                auto notification = std::make_shared<PartyNotificationPartyLeave>();
                notification->playerId = memberId;
                notification->partyName = party->GetName();
                notification->leaverName = player->GetName();

                _communityService.Notify(member->GetZoneId(), std::move(notification));
            }

            if (std::ssize(party->GetMembers()) == 1)
            {
                shouldDisband = true;
                autoDisband = true;
            }
        }

        if (shouldDisband)
        {
            for (int64_t memberId : party->GetMembers())
            {
                CommunityPlayer* member = playerStorage.Find(memberId);
                if (!member)
                {
                    assert(false);

                    continue;
                }

                member->SetPartyId(std::nullopt);

                auto notification = std::make_shared<PartyNotificationPartyDisband>();
                notification->playerId = memberId;
                notification->partyName = party->GetName();
                notification->autoDisband = autoDisband;

                _communityService.Notify(member->GetZoneId(), std::move(notification));
            }

            _parties.erase(party->GetId());
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

        _communityService.Notify(target->GetZoneId(), std::move(notification));
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

    auto CommunityPartyService::FindParty(int64_t partyId) -> Party*
    {
        const auto iter = _parties.find(partyId);

        return iter != _parties.end() ? &iter->second : nullptr;
    }

    auto CommunityPartyService::FindParty(int64_t partyId) const -> const Party*
    {
        const auto iter = _parties.find(partyId);

        return iter != _parties.end() ? &iter->second : nullptr;
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

    auto CommunityPartyService::CreatePartyPlayerInformationList(const Party& party) const -> std::vector<PartyPlayerInformation>
    {
        const CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();
        const std::vector<int64_t>& members = party.GetMembers();

        std::vector<PartyPlayerInformation> result;
        result.reserve(members.size());

        for (int64_t partyPlayerId : members)
        {
            const CommunityPlayer* partyPlayer = playerStorage.Find(partyPlayerId);
            if (!partyPlayerId)
            {
                assert(false);

                continue;
            }

            result.emplace_back(CreatePartyPlayerInformation(*partyPlayer));
        }

        return result;
    }
}

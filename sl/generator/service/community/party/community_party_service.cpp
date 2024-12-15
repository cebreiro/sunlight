#include "community_party_service.h"

#include "sl/generator/service/community/community_service.h"
#include "sl/generator/service/community/notification/party_notification.h"
#include "sl/generator/service/community/player/community_player_storage.h"

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
        if (!player.HasParty())
        {
            return;
        }

        PartyCommandPartyLeave command;
        command.playerId = player.GetId();

        HandleCommand(command);
    }

    void CommunityPartyService::OnPlayerReconnect(const CommunityPlayer& player)
    {
        if (!player.HasParty())
        {
            return;
        }

        const Party* party = FindParty(player.GetPartyId());
        if (!party)
        {
            assert(false);

            return;
        }

        auto notification = std::make_shared<PartyNotificationPartyJoinResult>();
        notification->playerId = player.GetId();
        notification->partyName = party->GetName();
        notification->result = ChannelJoinResult::Success;
        notification->players = CreatePartyPlayerInformationList(*party);
        notification->information = CreatePartyInformation(*party);

        _communityService.Notify(player.GetZoneId(), std::move(notification));
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
                notification->party = CreatePartyInformation(party);

                _communityService.Notify(playerStorage.Find(memberId)->GetZoneId(), std::move(notification));
            }

            return;
        }
        case ChannelInviteResult::AcceptPartyJoin:
        {
            ChannelJoinResult result = ChannelJoinResult::Success;
            std::string partyName;
            Party* party = nullptr;

            do
            {
                if (!inviter->HasParty())
                {
                    result = ChannelJoinResult::NotExist;

                    break;
                }

                party = FindParty(inviter->GetPartyId());
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

                ProcessPartyMemberAdd(*party, *invited);

                return;

            } while (false);

            auto notification = std::make_shared<PartyNotificationPartyJoinResult>();
            notification->playerId = invited->GetId();
            notification->partyName = std::move(partyName);
            notification->result = result;

            if (result == ChannelJoinResult::Success)
            {
                assert(party);

                notification->players = CreatePartyPlayerInformationList(*party);
            }

            _communityService.Notify(invited->GetZoneId(), std::move(notification));

            return;
        }
        }
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyList& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();
        const int64_t playerId = command.playerId;

        CommunityPlayer* player = playerStorage.Find(playerId);
        if (!player)
        {
            return;
        }

        _partyListingBuffer.clear();
        SelectPartyForListing(_partyListingBuffer);

        auto range = _partyListingBuffer | std::views::transform([this](PtrNotNull<const Party> party) -> PartyInformation
            {
                return CreatePartyInformation(*party);
            });

        auto notification = std::make_shared<PartyNotificationPartyList>();
        notification->playerId = command.playerId;
        notification->parties = std::ranges::to<std::vector>(range);

        _communityService.Notify(player->GetZoneId(), std::move(notification));
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
                notification->leaverId = player->GetId();

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
            ProcessDisbandment(*party, autoDisband);
        }
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyForceExit& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* player = playerStorage.Find(command.playerId);
        CommunityPlayer* target = playerStorage.FindByName(command.targetName);

        if (!player || !target || !player->HasParty() || !target->HasParty())
        {
            return;
        }

        if (player->GetPartyId() != target->GetPartyId())
        {
            return;
        }

        Party* party = FindParty(player->GetPartyId());
        if (!party || party->GetLeaderId() != player->GetId())
        {
            return;
        }

        Visit(*party, [this, party, target](const CommunityPlayer& member)
            {
                auto notification = std::make_shared<PartyNotificationPartyForceExit>();
                notification->playerId = member.GetId();
                notification->partyName = party->GetName();
                notification->targetName = target->GetName();
                notification->targetId = target->GetId();

                _communityService.Notify(member.GetZoneId(), std::move(notification));
            });

        party->Remove(target->GetId());
        target->SetPartyId(std::nullopt);

        if (const bool shouldDisband = std::ssize(party->GetMembers()) <= 1;
            shouldDisband)
        {
            ProcessDisbandment(*party, true);
        }
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyLeaderChange& command)
    {
        const CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* player = playerStorage.Find(command.playerId);
        const CommunityPlayer* newLeader = playerStorage.FindByName(command.newLeaderName);

        if (!player || !newLeader || !player->HasParty() || !newLeader->HasParty())
        {
            return;
        }

        if (player->GetPartyId() != newLeader->GetPartyId())
        {
            return;
        }

        Party* party = FindParty(player->GetPartyId());
        if (!party || party->GetLeaderId() != player->GetId())
        {
            return;
        }

        party->SetLeaderId(newLeader->GetId());

        for (int64_t memberId : party->GetMembers())
        {
            const CommunityPlayer* member = playerStorage.Find(memberId);
            if (!member)
            {
                assert(false);

                continue;
            }

            auto notification = std::make_shared<PartyNotificationPartyLeaderChange>();
            notification->playerId = memberId;
            notification->partyName = party->GetName();
            notification->newLeaderName = command.newLeaderName;

            _communityService.Notify(member->GetZoneId(), std::move(notification));
        }
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyOptionChange& command)
    {
        const CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* player = playerStorage.Find(command.playerId);

        if (!player || !player->HasParty())
        {
            return;
        }

        Party* party = FindParty(player->GetPartyId());
        if (!party || party->GetLeaderId() != player->GetId())
        {
            return;
        }

        if (party->IsPublic() == command.isPublic &&
            party->IsSetGoldDistribution() == command.setGoldDistribution &&
            party->IsSetItemDistribution() == command.setItemDistribution)
        {
            return;
        }

        party->SetPublic(command.isPublic);
        party->SetGoldDistribution(command.setGoldDistribution);
        party->SetItemDistribution(command.setItemDistribution);

        auto notification = std::make_shared<PartyNotificationPartyOptionChange>();
        notification->playerId = player->GetId();
        notification->party = CreatePartyInformation(*party);

        _communityService.Notify(player->GetZoneId(), std::move(notification));
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyJoin& command)
    {
        const CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* requester = playerStorage.Find(command.playerId);
        const CommunityPlayer* partyLeader = playerStorage.FindByName(command.targetName);

        if (!requester || !partyLeader)
        {
            return;
        }

        if (requester->HasParty() || !partyLeader->HasParty())
        {
            return;
        }

        Party* party = FindParty(partyLeader->GetPartyId());
        if (!party)
        {
            assert(false);

            return;
        }

        if (party->GetLeaderId() != partyLeader->GetId() || !party->IsPublic())
        {
            return;
        }

        auto notification = std::make_shared<PartyNotificationPartyJoinRequest>();
        notification->playerId = partyLeader->GetId();
        notification->requesterName = requester->GetName();

        _communityService.Notify(partyLeader->GetZoneId(), std::move(notification));
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyJoinAck& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* partyLeader = playerStorage.Find(command.playerId);
        CommunityPlayer* requester = playerStorage.FindByName(command.targetName);

        if (!partyLeader || !requester)
        {
            return;
        }

        if (!partyLeader->HasParty() || requester->HasParty())
        {
            return;
        }

        Party* party = FindParty(partyLeader->GetPartyId());
        if (!party)
        {
            assert(false);

            return;
        }

        if (party->GetLeaderId() != partyLeader->GetId())
        {
            return;
        }

        if (party->IsFull())
        {
            return;
        }

        ProcessPartyMemberAdd(*party, *requester);
    }

    void CommunityPartyService::HandleCommand(const PartyCommandPartyJoinReject& command)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        const CommunityPlayer* partyLeader = playerStorage.Find(command.playerId);
        const CommunityPlayer* requester = playerStorage.FindByName(command.targetName);

        if (!partyLeader || !requester)
        {
            return;
        }

        if (requester->HasParty())
        {
            return;
        }

        auto notification = std::make_shared<PartyNotificationPartyJoinRejected>();
        notification->playerId = requester->GetId();
        notification->partyLeaderName = partyLeader->GetName();

        _communityService.Notify(requester->GetZoneId(), std::move(notification));
    }

    void CommunityPartyService::BroadcastPartyChatting(int64_t partyId, const std::string& sender, const std::string& message)
    {
        const auto* party = FindParty(partyId);
        if (!party)
        {
            return;
        }

        Visit(*party, [&, this](const CommunityPlayer& player)
            {
                auto notification = std::make_shared<PartyNotificationPartyChat>();
                notification->playerId = player.GetId();
                notification->sender = sender;
                notification->message = message;

                _communityService.Notify(player.GetZoneId(), std::move(notification));
            });
    }

    void CommunityPartyService::ProcessPartyMemberAdd(Party& party, CommunityPlayer& newMember)
    {
        assert(!party.IsFull());
        assert(!newMember.HasParty());

        Visit(party, [this, &party, &newMember](const CommunityPlayer& member)
            {
                auto notification = std::make_shared<PartyNotificationPartyMemberAdd>();
                notification->playerId = member.GetId();
                notification->partyName = party.GetName();
                notification->member = CreatePartyPlayerInformation(newMember);

                _communityService.Notify(member.GetZoneId(), std::move(notification));
            });

        party.Add(newMember.GetId());
        newMember.SetPartyId(party.GetId());

        auto notification = std::make_shared<PartyNotificationPartyJoinResult>();
        notification->playerId = newMember.GetId();
        notification->partyName = party.GetName();
        notification->result = ChannelJoinResult::Success;
        notification->players = CreatePartyPlayerInformationList(party);

        _communityService.Notify(newMember.GetZoneId(), std::move(notification));
    }

    void CommunityPartyService::ProcessDisbandment(Party& party, bool isAutoDisbandment)
    {
        Visit(party, [this, &party, isAutoDisbandment](CommunityPlayer& member)
            {
                member.SetPartyId(std::nullopt);

                auto notification = std::make_shared<PartyNotificationPartyDisband>();
                notification->playerId = member.GetId();
                notification->partyName = party.GetName();
                notification->autoDisband = isAutoDisbandment;

                _communityService.Notify(member.GetZoneId(), std::move(notification));
            });

        _parties.erase(party.GetId());
    }

    void CommunityPartyService::Visit(const Party& party, const std::function<void(CommunityPlayer&)>& visitor)
    {
        CommunityPlayerStorage& playerStorage = _communityService.GetPlayerStorage();

        for (int64_t memberId : party.GetMembers())
        {
            CommunityPlayer* member = playerStorage.Find(memberId);
            if (!member)
            {
                assert(false);

                continue;
            }

            visitor(*member);
        }
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

    void CommunityPartyService::SelectPartyForListing(std::vector<PtrNotNull<const Party>>& result)
    {
        constexpr int64_t max_select_count = 16;

        int64_t count = 0;

        for (const Party& party : _parties | std::views::values)
        {
            if (party.IsPublic())
            {
                result.push_back(&party);

                if (++count >= max_select_count)
                {
                    break;
                }
            }
        }
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
            .cid = player.GetId(),
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

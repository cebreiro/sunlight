#include "player_channel_system.h"

#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/channel/game_channel_type.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/character_message.h"
#include "sl/emulator/game/message/zone_community_message.h"
#include "sl/emulator/game/message/creator/character_message_creator.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_community_service.h"
#include "sl/emulator/service/community/command/community_command.h"
#include "sl/emulator/service/community/command/party_command.h"
#include "sl/emulator/service/community/notification/party_notification.h"

namespace sunlight
{
    PlayerChannelSystem::PlayerChannelSystem(const ServiceLocator& serviceLocator, int32_t zoneId)
        : _serviceLocator(serviceLocator)
        , _zoneId(zoneId)
    {
    }

    void PlayerChannelSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<PlayerIndexSystem>());
    }

    bool PlayerChannelSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(CharacterMessageType::ChannelInvite,
            std::bind_front(&PlayerChannelSystem::HandleChannelInvite, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(CharacterMessageType::ChannelInviteResult,
            std::bind_front(&PlayerChannelSystem::HandleChannelInviteResult, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::CHANNEL_LEAVE,
            std::bind_front(&PlayerChannelSystem::HandleChannelLeave, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::FORCEEXIT,
            std::bind_front(&PlayerChannelSystem::HandleChannelForceExit, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::PARTY_CHANGE_LEADER,
            std::bind_front(&PlayerChannelSystem::HandlePartyLeaderChange, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::PARTY_CHANGE_OPTION,
            std::bind_front(&PlayerChannelSystem::HandlePartyOptionChange, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::CHANNEL_ENUM_USER,
            std::bind_front(&PlayerChannelSystem::HandleChannelUserList, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(CharacterMessageType::JoinReq,
            std::bind_front(&PlayerChannelSystem::HandlePartyJoinRequest, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(CharacterMessageType::JoinAck,
            std::bind_front(&PlayerChannelSystem::HandlePartyJoinAck, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(CharacterMessageType::JoinReje,
            std::bind_front(&PlayerChannelSystem::HandlePartyJoinReject, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(CharacterMessageType::WhereAreYou,
            std::bind_front(&PlayerChannelSystem::HandleWhereAreYou, this)))
        {
            return false;
        }

        return true;
    }

    auto PlayerChannelSystem::GetName() const -> std::string_view
    {
        return "player_channel_system";
    }

    auto PlayerChannelSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerChannelSystem>();
    }

    void PlayerChannelSystem::OnStageEnter(const GamePlayer& player, StageEnterType enterType)
    {
        if (enterType == StageEnterType::Login)
        {
            auto command = std::make_shared<CommunityCommandPlayerRegister>();
            command->zoneId = _zoneId;
            command->playerId = player.GetCId();
            command->name = player.GetName();

            const Job& mainJob = player.GetJobComponent().GetMainJob();
            const PlayerStatComponent& statComponent = player.GetStatComponent();
            
            command->jobId = static_cast<int32_t>(mainJob.GetId());
            command->jobLevel = static_cast<int8_t>(mainJob.GetLevel());
            command->characterLevel = static_cast<int8_t>(statComponent.GetLevel());
            command->maxHP = statComponent.GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();

            _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
        }
    }

    void PlayerChannelSystem::OnStageExit(const GamePlayer& player, StageExitType exitType)
    {
        switch (exitType)
        {
        case StageExitType::Logout:
        {
            auto command = std::make_shared<CommunityCommandPlayerDeregister>();
            command->zoneId = _zoneId;
            command->playerId = player.GetCId();

            _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
        }
        break;
        case StageExitType::ZoneChange:
        {
            auto command = std::make_shared<CommunityCommandPlayerDeregisterTimer>();
            command->zoneId = _zoneId;
            command->playerId = player.GetCId();
            command->milliseconds = 3000;

            _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
        }
        break;
        case StageExitType::StageChange:
        default:;
        }
    }

    void PlayerChannelSystem::UpdateCommunityPlayerStat(const GamePlayer& player)
    {
        const Job& mainJob = player.GetJobComponent().GetMainJob();
        const PlayerStatComponent& statComponent = player.GetStatComponent();

        auto command = std::make_shared<CommunityCommandPlayerUpdateInformation>();
        command->zoneId = _zoneId;
        command->playerId = player.GetCId();
        command->jobId = static_cast<int32_t>(mainJob.GetId());
        command->jobLevel = static_cast<int8_t>(mainJob.GetLevel());
        command->characterLevel = static_cast<int8_t>(statComponent.GetLevel());
        command->maxHP = statComponent.GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationCreateResult& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        // TODO: Set Party information

        player->Send(CharacterMessageCreator::CreatePartyCreate(notification.party,
            notification.leader, notification.member));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyList& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Defer(CharacterMessageCreator::CreatePartyList(notification.parties));
        player->FlushDeferred();
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyInvite& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyInvite(
            notification.inviterName, notification.partyName, "", notification.creation ? 1: 0));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyInviteRefused& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyInviteResult(notification.refuserName, notification.result));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyJoinResult& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        if (notification.information.has_value())
        {
            player->Defer(CharacterMessageCreator::CreatePartyCreate(*notification.information, notification.players[0], notification.players[1]));
        }
        else
        {
            player->Defer(CharacterMessageCreator::CreateJoinResult(notification.partyName, notification.result));
        }

        if (notification.result == ChannelJoinResult::Success)
        {
            player->Defer(CharacterMessageCreator::CreatePartyQueryResult(notification.partyName, notification.players));
        }

        player->FlushDeferred();
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyMemberAdd& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        // TODO: update party information

        player->Defer(CharacterMessageCreator::CreatePartyMemberAdd(notification.partyName, notification.member));
        player->Defer(CharacterMessageCreator::CreatePartyMemberEnterNotify(notification.partyName, notification.member.name));
        player->FlushDeferred();
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyLeave& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        // TODO: remove party member

        player->Send(CharacterMessageCreator::CreatePartyMemberLeave(notification.partyName, notification.leaverName));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyDisband& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        // TODO: remove party data all

        player->Send(CharacterMessageCreator::CreatePartyDisband(notification.partyName, notification.autoDisband));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyForceExit& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        // TODO: remove party member data

        player->Send(CharacterMessageCreator::CreatePartyForceExit(notification.partyName, notification.targetName));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyLeaderChange& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyLeaderChange(notification.partyName, notification.newLeaderName));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyOptionChange& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyOptionChange(notification.party));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyJoinRequest& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyJoinRequest(notification.requesterName));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyJoinRejected& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyJoinRejected(notification.partyLeaderName));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyPlayerStateRequested& notification)
    {
        const GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        auto command = std::make_shared<PartyCommandPartyPlayerStateResponse>();
        command->playerId = player->GetCId();
        command->requestId = notification.requesterId;

        const Eigen::Vector2f position = player->GetSceneObjectComponent().GetPosition();
        command->x = position.x();
        command->y = position.y();
        command->hp = player->GetStatComponent().GetFinalStat(RecoveryStatType::HP).As<float>();

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyPlayerState& notification)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreateIamHere(notification.targetName, 0, notification.x, notification.y, notification.hp));
    }

    void PlayerChannelSystem::HandleChannelInvite(const CharacterMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const std::string channelName = reader.ReadString();

        [[maybe_unused]]
        const std::string unk1 = reader.ReadString();

        const GameChannelType channelType = static_cast<GameChannelType>(reader.Read<int8_t>());
        const int8_t create = reader.Read<int8_t>();

        switch (channelType)
        {
        case GameChannelType::Party:
        {
            if (create)
            {
                auto command = std::make_shared<PartyCommandCreate>();
                command->playerId = player.GetCId();
                command->partyName = channelName;
                command->targetName = message.targetName;

                _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
            }
            else
            {
                auto command = std::make_shared<PartyCommandInvite>();
                command->playerId = player.GetCId();
                command->targetName = message.targetName;

                _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
            }
        }
        break;
        case GameChannelType::Channel:
        case GameChannelType::Guild:
        {
            // TODO impl
            assert(false);
        }
        break;
        }
    }

    void PlayerChannelSystem::HandleChannelInviteResult(const CharacterMessage& message)
    {
        auto command = std::make_shared<PartyCommandInviteResult>();
        command->playerId = message.player.GetCId();
        command->inviterName = message.targetName;
        command->result = static_cast<ChannelInviteResult>(message.reader.Read<int8_t>());

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }

    void PlayerChannelSystem::HandleChannelLeave(const ZoneCommunityMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const std::string channelName = reader.ReadString();
        const GameChannelType channelType = static_cast<GameChannelType>(reader.Read<int8_t>());

        switch (channelType)
        {
        case GameChannelType::Party:
        {
            auto command = std::make_shared<PartyCommandPartyLeave>();
            command->playerId = player.GetCId();

            _serviceLocator.Get<GameCommunityService>().Send(std::move(command));

            player.Send(CharacterMessageCreator::CreatePartyLeave());
        }
        break;
        case GameChannelType::Channel:
        case GameChannelType::Guild:
        default:
            assert(false);
        }
    }

    void PlayerChannelSystem::HandleChannelForceExit(const ZoneCommunityMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const std::string channelName = reader.ReadString();
        std::string targetName = reader.ReadString();
        const GameChannelType channelType = static_cast<GameChannelType>(reader.Read<int8_t>());

        switch (channelType)
        {
        case GameChannelType::Party:
        {
            auto command = std::make_shared<PartyCommandPartyForceExit>();
            command->playerId = player.GetCId();
            command->targetName = std::move(targetName);

            _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
        }
        break;
        case GameChannelType::Channel:
        case GameChannelType::Guild:
        default:
            assert(false);
        }
    }

    void PlayerChannelSystem::HandlePartyLeaderChange(const ZoneCommunityMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const std::string partyName = reader.ReadString();
        std::string newLeaderName = reader.ReadString();

        auto command = std::make_shared<PartyCommandPartyLeaderChange>();
        command->playerId = player.GetCId();
        command->newLeaderName = std::move(newLeaderName);

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }

    void PlayerChannelSystem::HandlePartyOptionChange(const ZoneCommunityMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const std::string partyName = reader.ReadString();

        BufferReader partyObjectReader = reader.ReadObject();

        constexpr int64_t party_object_size = 332;
        if (!partyObjectReader.CanRead(party_object_size))
        {
            return;
        }

        partyObjectReader.Skip(324);

        const bool isPublic = static_cast<bool>(partyObjectReader.Read<int8_t>());
        const bool setGoldDistribution = static_cast<bool>(partyObjectReader.Read<int8_t>());
        const bool setItemDistribution = static_cast<bool>(partyObjectReader.Read<int8_t>());

        auto command = std::make_shared<PartyCommandPartyOptionChange>();
        command->playerId = player.GetCId();
        command->isPublic = isPublic;
        command->setGoldDistribution = setGoldDistribution;
        command->setItemDistribution = setItemDistribution;

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }

    void PlayerChannelSystem::HandleChannelUserList(const ZoneCommunityMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const GameChannelType channelType = static_cast<GameChannelType>(reader.Read<int8_t>());

        switch (channelType)
        {
        case GameChannelType::Party:
        {
            auto command = std::make_shared<PartyCommandPartyList>();
            command->playerId = player.GetCId();

            _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
        }
        break;
        case GameChannelType::Channel:
        case GameChannelType::Guild:
        default:
            assert(false);
        }
    }

    void PlayerChannelSystem::HandlePartyJoinRequest(const CharacterMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;

        const GameChannelType channelType = static_cast<GameChannelType>(reader.Read<int8_t>());

        switch (channelType)
        {
        case GameChannelType::Party:
        {
            auto command = std::make_shared<PartyCommandPartyJoin>();
            command->playerId = player.GetCId();
            command->targetName = message.targetName;

            _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
        }
        break;
        case GameChannelType::Channel:
        case GameChannelType::Guild:
        default:
            assert(false);
        }
    }

    void PlayerChannelSystem::HandlePartyJoinAck(const CharacterMessage& message)
    {
        auto command = std::make_shared<PartyCommandPartyJoinAck>();
        command->playerId = message.player.GetCId();
        command->targetName = message.targetName;

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }

    void PlayerChannelSystem::HandlePartyJoinReject(const CharacterMessage& message)
    {
        auto command = std::make_shared<PartyCommandPartyJoinReject>();
        command->playerId = message.player.GetCId();
        command->targetName = message.targetName;

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }

    void PlayerChannelSystem::HandleWhereAreYou(const CharacterMessage& message)
    {
        SlPacketReader& reader = message.reader;

        const int8_t type = reader.Read<int8_t>();
        if (type == 1)
        {
            // user command using chatting
            // I think that it has no reason to allow
            return;
        }

        // when client is in party play, constantly request to show party member's hp

        auto command = std::make_shared<PartyCommandPartyPlayerStateRequest>();
        command->playerId = message.player.GetCId();
        command->targetName = message.targetName;

        _serviceLocator.Get<GameCommunityService>().Send(std::move(command));
    }
}

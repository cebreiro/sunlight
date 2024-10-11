#include "player_channel_system.h"

#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/contents/channel/game_channel_type.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/character_message.h"
#include "sl/emulator/game/message/creator/character_message_creator.h"
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
        Add(stage.Get<SceneObjectSystem>());
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
            
            command->jobId = static_cast<int32_t>(mainJob.GetId());
            command->jobLevel = static_cast<int8_t>(mainJob.GetLevel());
            command->characterLevel = static_cast<int8_t>(player.GetStatComponent().GetLevel());

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

    void PlayerChannelSystem::HandleNotification(const PartyNotificationCreateResult& notification)
    {
        GamePlayer* player = Get<SceneObjectSystem>().FindPlayerByCid(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyCreate(notification.information,
            notification.leader, notification.member));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyInvite& notification)
    {
        GamePlayer* player = Get<SceneObjectSystem>().FindPlayerByCid(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyInvite(
            notification.inviterName, notification.partyName, "", notification.creation ? 1: 0));
    }

    void PlayerChannelSystem::HandleNotification(const PartyNotificationPartyInviteRefused& notification)
    {
        GamePlayer* player = Get<SceneObjectSystem>().FindPlayerByCid(notification.playerId);
        if (!player)
        {
            return;
        }

        player->Send(CharacterMessageCreator::CreatePartyInviteResult(notification.refuserName, notification.result));
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
}

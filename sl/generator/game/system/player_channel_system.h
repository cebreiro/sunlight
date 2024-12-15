#pragma once
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/game_system.h"
#include "sl/generator/game/zone/stage_enter_type.h"
#include "sl/generator/game/zone/stage_exit_type.h"
#include "sl/generator/service/community/community_chat_type.h"

namespace sunlight
{
    struct CharacterMessage;
    struct ZoneMessage;
    struct ZoneCommunityMessage;

    struct PartyNotificationCreateResult;
    struct PartyNotificationPartyList;
    struct PartyNotificationPartyInvite;
    struct PartyNotificationPartyInviteRefused;
    struct PartyNotificationPartyJoinResult;
    struct PartyNotificationPartyMemberAdd;
    struct PartyNotificationPartyLeave;
    struct PartyNotificationPartyDisband;
    struct PartyNotificationPartyForceExit;
    struct PartyNotificationPartyLeaderChange;
    struct PartyNotificationPartyOptionChange;
    struct PartyNotificationPartyJoinRequest;
    struct PartyNotificationPartyJoinRejected;
    struct PartyNotificationPartyChat;
    struct CommunityNotificationWhisperChat;
    struct CommunityNotificationWhisperChatFail;
    struct CommunityNotificationGlobalChat;
}

namespace sunlight
{
    class PlayerChannelSystem final : public GameSystem
    {
    public:
        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnStageEnter(const GamePlayer& player, StageEnterType enterType);
        void OnStageExit(const GamePlayer& player, StageExitType exitType);

    public:
        PlayerChannelSystem(const ServiceLocator& serviceLocator, int32_t zoneId);

        void UpdateCommunityPlayerStat(const GamePlayer& player);

    public:
        void HandleNotification(const PartyNotificationCreateResult& notification);
        void HandleNotification(const PartyNotificationPartyList& notification);
        void HandleNotification(const PartyNotificationPartyInvite& notification);
        void HandleNotification(const PartyNotificationPartyInviteRefused& notification);
        void HandleNotification(const PartyNotificationPartyJoinResult& notification);
        void HandleNotification(const PartyNotificationPartyMemberAdd& notification);
        void HandleNotification(const PartyNotificationPartyLeave& notification);
        void HandleNotification(const PartyNotificationPartyDisband& notification);
        void HandleNotification(const PartyNotificationPartyForceExit& notification);
        void HandleNotification(const PartyNotificationPartyLeaderChange& notification);
        void HandleNotification(const PartyNotificationPartyOptionChange& notification);
        void HandleNotification(const PartyNotificationPartyJoinRequest& notification);
        void HandleNotification(const PartyNotificationPartyJoinRejected& notification);
        void HandleNotification(const PartyNotificationPartyChat& notification);
        void HandleNotification(const CommunityNotificationWhisperChat& notification);
        void HandleNotification(const CommunityNotificationWhisperChatFail& notification);
        void HandleNotification(const CommunityNotificationGlobalChat& notification);

    private:
        void HandleChannelInvite(const CharacterMessage& message);
        void HandleChannelInviteResult(const CharacterMessage& message);
        void HandleChannelLeave(const ZoneCommunityMessage& message);
        void HandleChannelForceExit(const ZoneCommunityMessage& message);
        void HandlePartyLeaderChange(const ZoneCommunityMessage& message);
        void HandlePartyOptionChange(const ZoneCommunityMessage& message);
        void HandleChannelUserList(const ZoneCommunityMessage& message);
        void HandlePartyJoinRequest(const CharacterMessage& message);
        void HandlePartyJoinAck(const CharacterMessage& message);
        void HandlePartyJoinReject(const CharacterMessage& message);
        void HandleWhereAreYou(const CharacterMessage& message);

        void HandleChatNormal(const ZoneMessage& message);
        void HandleChannelChat(const ZoneCommunityMessage& message);
        void HandleChatWhisper(const CharacterMessage& message);
        void HandleChatShout(const ZoneCommunityMessage& message);
        void HandleChatTrade(const ZoneCommunityMessage& message);
        void HandleChatEcho(const ZoneCommunityMessage& message);

    private:
        void SendChatCommand(CommunityChatType type, int64_t playerId, std::string message, std::optional<std::string> target = std::nullopt);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _zoneId = 0;
    };
}

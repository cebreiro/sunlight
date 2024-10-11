#pragma once
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/game/zone/stage_enter_type.h"
#include "sl/emulator/game/zone/stage_exit_type.h"

namespace sunlight
{
    struct CharacterMessage;
    struct ZoneCommunityMessage;

    struct PartyNotificationCreateResult;
    struct PartyNotificationPartyInvite;
    struct PartyNotificationPartyInviteRefused;
    struct PartyNotificationPartyJoinResult;
    struct PartyNotificationPartyMemberAdd;
    struct PartyNotificationPartyLeave;
    struct PartyNotificationPartyDisband;
    struct PartyNotificationPartyPlayerStateRequested;
    struct PartyNotificationPartyPlayerState;
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
        void HandleNotification(const PartyNotificationPartyInvite& notification);
        void HandleNotification(const PartyNotificationPartyInviteRefused& notification);
        void HandleNotification(const PartyNotificationPartyJoinResult& notification);
        void HandleNotification(const PartyNotificationPartyMemberAdd& notification);
        void HandleNotification(const PartyNotificationPartyLeave& notification);
        void HandleNotification(const PartyNotificationPartyDisband& notification);
        void HandleNotification(const PartyNotificationPartyPlayerStateRequested& notification);
        void HandleNotification(const PartyNotificationPartyPlayerState& notification);


    private:
        void HandleChannelInvite(const CharacterMessage& message);
        void HandleChannelInviteResult(const CharacterMessage& message);
        void HandleChannelLeave(const ZoneCommunityMessage& message);
        void HandleWhereAreYou(const CharacterMessage& message);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _zoneId = 0;
    };
}

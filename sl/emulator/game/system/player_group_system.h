#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;
    struct ZoneCommunityMessage;

    class GameGroup;
    class GamePlayer;
}

namespace sunlight
{
    class PlayerGroupSystem final : public GameSystem
    {
    public:
        explicit PlayerGroupSystem(const ServiceLocator& serviceLocator);
        ~PlayerGroupSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnStageExit(GamePlayer& player);

    private:
        void HandleStateProposition(const ZoneMessage& message);
        void HandleCreateGroup(const ZoneCommunityMessage& message);
        void HandleGroupMessage(const ZoneCommunityMessage& message);

    private:
        void OnHostExit(GameGroup& group, GamePlayer& host);
        void OnGuestExit(GameGroup& group, GamePlayer& guest);

    private:
        const ServiceLocator& _serviceLocator;

        int32_t _nextGroupId = 0;
        std::unordered_map<int32_t, UniquePtrNotNull<GameGroup>> _gameGroups;
    };
}

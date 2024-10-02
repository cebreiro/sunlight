#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight::sox
{
    struct Zone;
}

namespace sunlight
{
    struct PlayerProfileIntroduction;
    struct CharacterMessage;
    struct ZoneMessage;

    class GamePlayer;
}

namespace sunlight
{
    class PlayerProfileSystem final : public GameSystem
    {
    public:
        PlayerProfileSystem(const ServiceLocator& serviceLocator, int32_t zoneId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnStageEnter(GamePlayer& player);
        void OnStageExit(const GamePlayer& player);
        void OnZoneExit(const GamePlayer& player);

    public:
        void OnProfileSettingChange(const ZoneMessage& message);

    public:
        void HandleShowYou(const CharacterMessage& message);
        void HandleProfileMessage(const ZoneMessage& message);

        void HandleProfileIntroductionRequest(GamePlayer& player, const std::string& targetName);
        void OnProfileIntroductionLoadComplete(const std::string& playerName, const std::string& targetName, PlayerProfileIntroduction& introduction);

        void HandleProfileIntroductionSave(const ZoneMessage& message);

    private:
        auto FindPlayer(const std::string& name) -> GamePlayer*;
        auto FindPlayer(const std::string& name) const -> const GamePlayer*;

    private:
        const ServiceLocator& _serviceLocator;
        const sox::Zone& _zoneData;

        std::unordered_map<std::string, GamePlayer*> _playerNameIndex;
    };
}

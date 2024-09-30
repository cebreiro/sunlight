#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight::sox
{
    struct Zone;
}

namespace sunlight
{
    struct CharacterMessage;

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

    private:
        void HandleShowYou(const CharacterMessage& message);

    private:
        const ServiceLocator& _serviceLocator;
        const sox::Zone& _zoneData;

        std::unordered_map<std::string, GamePlayer*> _playerNameIndex;
    };
}

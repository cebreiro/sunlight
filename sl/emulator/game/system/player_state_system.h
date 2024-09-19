#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;

    class GamePlayer;
}

namespace sunlight
{
    class PlayerStateSystem final : public GameSystem
    {
    public:
        explicit PlayerStateSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:


    private:
        bool HandleCharacterState(const ZoneMessage& message);

        void HandlePickGroundItem(GamePlayer& player, game_entity_id_type itemId);

    private:
        const ServiceLocator& _serviceLocator;
    };
}

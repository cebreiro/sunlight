#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GamePlayer;
    struct ZoneMessage;
}

namespace sunlight
{
    class EntityInitializationSystem final : public GameSystem
    {
    public:
        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void Initialize(GamePlayer& player);

    private:
        void HandlePlayerAllState(const ZoneMessage& message);
        void HandlePlayerActivate(const ZoneMessage& message);
    };
}

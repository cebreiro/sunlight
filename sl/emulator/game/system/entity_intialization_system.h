#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;
}

namespace sunlight
{
    class EntityInitializationSystem final : public GameSystem
    {
    public:
        bool Subscribe(Stage& stage) override;

        auto GetClassId() const -> game_system_id_type override;

    private:
        void HandlePlayerAllState(const ZoneMessage& message);
        void HandlePlayerActivate(const ZoneMessage& message);
    };
}

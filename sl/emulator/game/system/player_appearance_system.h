#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class PlayerAppearanceSystem final : public GameSystem
    {
    public:
        explicit PlayerAppearanceSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        void RefreshWeaponMotionCategory(GamePlayer& player);

    private:
        auto GetWeaponMotionCategory(const GamePlayer& player) const -> int32_t;

    private:
        const ServiceLocator& _serviceLocator;
    };
}

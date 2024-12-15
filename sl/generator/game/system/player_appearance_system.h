#pragma once
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;

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

    public:
        void UpdateEquipmentAppearance(GamePlayer& player);

    private:
        void HandleMultiPlayerSyncMessage(const ZoneMessage& message);

        void HandleHairColorChange(GamePlayer& player, int32_t newColor);
        void HandleHairChange(GamePlayer& player, int32_t newHair);

    private:
        auto GetWeaponMotionCategory(const GamePlayer& player) const -> int32_t;

    private:
        const ServiceLocator& _serviceLocator;
    };
}

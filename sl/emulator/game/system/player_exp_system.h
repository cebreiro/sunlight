#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class PlayerExpSystem : public GameSystem
    {
    public:
        explicit PlayerExpSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void GainExp(GamePlayer& player, int32_t exp);
        void GainCharacterExp(GamePlayer& player, int32_t exp);
        void GainJobExp(GamePlayer& player, int32_t exp);

    private:
        const ServiceLocator& _serviceLocator;
    };
}

#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GameGroup;

    class GamePlayer;
}

namespace sunlight
{
    class ItemTradeSystem final : public GameSystem
    {
    public:
        explicit ItemTradeSystem(const ServiceLocator& serviceLocator);

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void Start(GamePlayer& player);

        bool Rollback(GamePlayer& player);
        bool Commit(GamePlayer& host, GamePlayer& guest);

        bool LiftItem(GameGroup& group, GamePlayer& player, game_entity_id_type itemId);
        bool LowerItem(GameGroup& group, GamePlayer& player, int32_t x, int32_t y, game_entity_id_type itemId);

        bool ChangeGold(GameGroup& group, GamePlayer& player, int32_t gold);

    private:
        const ServiceLocator& _serviceLocator;
    };
}

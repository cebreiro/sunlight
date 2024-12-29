#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;

    class GamePlayer;
    class GameNPC;
}

namespace sunlight
{
    class NPCShopSystem final
        : public GameSystem
        , public std::enable_shared_from_this<NPCShopSystem>
    {
    public:
        explicit NPCShopSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void InitializeItemShop(GameNPC& npc);

        bool Roll(GameNPC& npc);

        void OnStageExit(GamePlayer& player);

        void OpenItemShop(GamePlayer& player, GameNPC& npc);

        void OnShopSynchronizeRequest(const ZoneMessage& message);
        void OnShopSynchronizeStop(const ZoneMessage& message);

        void OnPlayerBuyShopItem(const ZoneMessage& message);
        void OnPlayerSellOwnItem(const ZoneMessage& message);

        void OnBarberPayment(const ZoneMessage& message);

    private:
        void CreateShopItems(GameNPC& npc);

        void ConfigureNPCShopRollTimer(GameNPC& npc);
        void OnRollTimerEnd(game_entity_id_type npcId);

    private:
        const ServiceLocator& _serviceLocator;

        std::mt19937 _mt19937;
    };
}

#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;

    class GamePlayer;
    class GameNPC;
}

namespace sunlight
{
    class NPCShopSystem final : public GameSystem
    {
    public:
        explicit NPCShopSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void InitializeItemShop(GameNPC& npc);

        void OnStageExit(GamePlayer& player);

        void OpenItemShop(GamePlayer& player, GameNPC& npc);

        void OnShopSynchronizeRequest(const ZoneMessage& message);
        void OnShopSynchronizeStop(const ZoneMessage& message);

        void OnPlayerBuyShopItem(const ZoneMessage& message);
        void OnPlayerSellOwnItem(const ZoneMessage& message);

    private:
        const ServiceLocator& _serviceLocator;

        std::mt19937 _mt19937;

        std::unordered_map<PtrNotNull<GamePlayer>, PtrNotNull<GameNPC>> _targetNPC;
    };
}

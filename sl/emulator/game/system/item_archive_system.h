#pragma once
#include "sl/data/sox/item/equipment_type.h"
#include "sl/emulator/game/contants/group/item_mix_material.h"
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/game/contants/item/item_remove_result.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight::db::dto
{
    struct AccountStorage;
}

namespace sunlight
{
    enum class ZoneMessageType : int32_t;

    struct ItemMixMaterial;
    struct ZoneMessage;

    class ItemData;
    class GameEntityIdPool;
    class GameItem;
    class GamePlayer;
    class GameNPC;
}

namespace sunlight
{
    class ItemArchiveSystem final
        : public GameSystem
        , public std::enable_shared_from_this<ItemArchiveSystem>
    {
    public:
        explicit ItemArchiveSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool PurchaseStreetVendorItem(GamePlayer& host, GamePlayer& guest, game_entity_id_type itemId, int32_t price);

        void Purchase(GamePlayer& player, GameNPC& npc, game_entity_id_type targetId, int32_t itemId, int32_t page, int32_t x, int32_t y, int32_t quantity);
        void SellOwnItem(GamePlayer& player, GameNPC& npc, game_entity_id_type targetId);

        bool AddItem(GamePlayer& player, int32_t itemId, int32_t quantity);
        bool AddItem(GamePlayer& player, SharedPtrNotNull<GameItem> item);

        bool GainItem(GamePlayer& player, int32_t itemId, int32_t quantity, int32_t& addedQuantity);
        bool GainItem(GamePlayer& player, SharedPtrNotNull<GameItem> item, int32_t& addedQuantity);

        bool RemoveInventoryItem(GamePlayer& player, int32_t itemId, int32_t quantity);

        bool Charge(GamePlayer& player, int32_t cost);

        void OpenAccountStorage(GamePlayer& player);

    public:
        void OnItemMixSuccess(GamePlayer& player, const ItemData& resultItemData, int32_t resultItemCount, std::span<const ItemMixMaterial> materials);
        void OnItemMixFail(GamePlayer& player, std::span<const ItemMixMaterial> materials);
        bool OnVendorSaleStorageClick(GamePlayer& player, int32_t index);

    public:
        static bool IsValid(EquipmentPosition position, sox::EquipmentType soxType);

    public:
        void OnWeaponSwap(const ZoneMessage& message);
        void TryRollbackMixItem(GamePlayer& player);
        bool OnMixItemLower(GamePlayer& player, game_entity_id_type itemId);
        bool OnMixItemLift(GamePlayer& player, game_entity_id_type itemId);

    private:
        void OnCompleteLoadAccountStorage(int64_t cid, const db::dto::AccountStorage& dto);

    private:
        void HandleMessage(const ZoneMessage& message);

        bool HandleLiftItem(GamePlayer& player, game_entity_id_type pick, int32_t quantity);
        bool HandleLowerItem(GamePlayer& player, game_entity_id_type lowerItemId, game_entity_id_type invenItemId,
            int8_t page, int8_t x, int8_t y);
        bool HandleLowerItemToQuickSlot(GamePlayer& player, game_entity_id_type lowerItemId, game_entity_id_type quickSlotItemId,
            int8_t page, int8_t x, int8_t y);

        bool HandleLiftEquipment(GamePlayer& player, EquipmentPosition position);
        bool HandleLowerEquipment(GamePlayer& player, game_entity_id_type equipItemId, EquipmentPosition position);

        bool HandleDestroyPickedItem(GamePlayer& player, game_entity_id_type pickedItemId);
        bool HandleDropPickedItem(GamePlayer& player, game_entity_id_type pickedItemId);

        bool HandleAccountStorageLowerItem(GamePlayer& player, int8_t page, int8_t x, int8_t y, game_entity_id_type pickedItemId);

    private:
        auto CreateNewGameItem(const ItemData& itemData, int32_t quantity) -> SharedPtrNotNull<GameItem>;

    private:
        void SaveChanges(GamePlayer& player);

    private:
        void LogHandleItemError(const char* func, const std::string& message) const;

        bool IsExternItemTransactionRunning(const GamePlayer& player) const;
        bool IsAllowed(const GamePlayer& player, ZoneMessageType itemMessage) const;

    private:
        const ServiceLocator& _serviceLocator;

        std::vector<item_remove_result_type> _itemRemoveResult;
    };
}

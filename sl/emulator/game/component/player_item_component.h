#pragma once
#include <boost/unordered/unordered_flat_set.hpp>
#include "sl/emulator/game/debug/game_debug_type.h"
#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/item/item_remove_result.h"
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/game/contants/item/inventory_position.h"
#include "sl/emulator/game/contants/item/item_position.h"
#include "sl/emulator/game/contants/item/item_slot_storage_base.h"
#include "sl/emulator/game/contants/quick_slot/quick_slot_position.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/service/database/dto/character.h"
#include "sl/emulator/service/database/transaction/item/item_log.h"

namespace sunlight
{
    class GameEntityIdPublisher;
    class GameItem;
    class GameEntityIdPool;
    class ItemData;
    class ItemDataProvider;
    class ItemSlotStorage;
}

namespace sunlight
{
    class PlayerItemComponent final : public GameComponent
    {
    public:
        PlayerItemComponent(GameEntityIdPublisher& idPublisher, const ItemDataProvider& itemDataProvider, const db::dto::Character& dto);
        ~PlayerItemComponent();

    public:
        bool HasItemLog() const;

        void FlushItemLogTo(std::vector<db::ItemLog>& dest);

    public:
        bool IsEmpty(int8_t page, const ItemSlotRange& range) const;
        bool IsEquipped(EquipmentPosition position) const;
        bool HasInventoryItem(int32_t itemId, int32_t quantity) const;

    public:
        void AddOrSubGold(int32_t value);
        void SetGold(int32_t value);

    public:
        bool AddInventoryItem(SharedPtrNotNull<GameItem> item, const InventoryPosition* hint = nullptr);
        bool AddQuickSlotItem(SharedPtrNotNull<GameItem> item, const QuickSlotPosition* hint = nullptr);

        bool TryRemoveInventoryItem(int32_t itemId, int32_t quantity, std::vector<item_remove_result_type>* result);

        bool TryStackItem(int32_t itemId, int32_t quantity,
            int32_t& usedQuantity, std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>>* result = nullptr);
        bool TryStackQuickSlotItem(const ItemData& itemData, int32_t quantity,
            int32_t& usedQuantity, std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>>* result = nullptr);

    public:
        bool LiftEquipItem(EquipmentPosition position);
        bool LowerPickedItemTo(EquipmentPosition position);
        bool SwapPickedItemTo(EquipmentPosition position);

        bool LiftItem(game_entity_id_type itemId);
        bool LowerPickedItemTo(int8_t page, int8_t x, int8_t y);
        bool SwapPickedItemTo(int8_t page, int8_t x, int8_t y);

        bool LowerPickedItemToQuickSlot(int8_t page, int8_t x, int8_t y);
        bool SwapPickedItemToQuickSlot(int8_t page, int8_t x, int8_t y);

        bool SwapWeaponItem();

    public:
        bool SetItemQuantity(game_entity_id_type id, int32_t quantity);
        bool IncreaseItemQuantity(game_entity_id_type id, int32_t quantity);
        bool DecreaseItemQuantity(game_entity_id_type id, int32_t quantity);

        bool AddNewPickedItem(SharedPtrNotNull<GameItem> item);
        bool RemovePickedItem();
        bool RemoveItem(game_entity_id_type id);
        auto ReleaseItem(game_entity_id_type id) -> std::shared_ptr<GameItem>;
        auto ReleaseItemByUId(int64_t id) -> std::shared_ptr<GameItem>;

        auto GetInventoryMasks() const -> std::vector<ItemSlotStorageBase<bool>>;

    public:
        auto FindItemShared(game_entity_id_type id) -> std::shared_ptr<GameItem>;

        auto FindItem(game_entity_id_type id) const -> const GameItem*;
        auto FindEquipmentItem(EquipmentPosition position) const -> const GameItem*;
        auto FindInventoryItem(game_entity_id_type id) const -> const GameItem*;
        auto FindQuickSlotItem(game_entity_id_type id) const -> const GameItem*;
        auto FindEmptyInventoryPosition(int32_t width, int32_t height) const -> std::optional<InventoryPosition>;
        auto FindEmptyQuickSlotPosition() const -> std::optional<QuickSlotPosition>;

    public:
        auto GetGold() const -> int32_t;
        auto GetInventoryPage() const -> int32_t;
        auto GetPickedItem() const -> const GameItem*;
        auto GetEquipmentItem(EquipmentPosition position) const -> const GameItem*;

        inline auto GetItemRange() const;
        inline auto GetEquipItems() const;

    private:
        auto Mutable(EquipmentPosition position) -> GameItem*&;

        auto GetInventorySlotStorage(int8_t page) -> ItemSlotStorage*;
        auto GetInventorySlotStorage(int8_t page) const -> const ItemSlotStorage*;
        auto GetQuickSlotStorage(int8_t page) -> ItemSlotStorage*;

    private:
        static auto ConvertToItemPosType(ItemPositionType position) -> db::ItemPosType;

        void AddItemUpdatePositionLog(const GameItem& item);
        void AddItemUpdateQuantityLog(const GameItem& item);
        void AddItemAddLog(const GameItem& item);
        void AddItemRemoveLog(const GameItem& item);
        void AddGoldChangeLog(int32_t newGold);

    private:
        int64_t _cid = 0;
        std::vector<db::ItemLog> _itemLogs;

        int32_t _gold = 0;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<GameItem>> _items;
        std::unordered_map<int64_t, PtrNotNull<GameItem>> _itemsUIdIndex;

        int32_t _inventoryPage = 0;
        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_INVENTORY_PAGE_SIZE> _inventorySlot;
        boost::unordered::unordered_flat_set<PtrNotNull<GameItem>> _itemStorageQueryResult;

        std::array<GameItem*, static_cast<int32_t>(EquipmentPosition::Count)> _equipments = {};

        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_QUICK_SLOT_PAGE_SIZE> _quickSlotStorages;

        GameItem* _pickItem = nullptr;

        static constexpr GameDebugType debug_type = GameDebugType::PlayerItemSlot;
    };

    inline auto PlayerItemComponent::GetItemRange() const
    {
        return _items | std::views::values | notnull::reference;
    }

    inline auto PlayerItemComponent::GetEquipItems() const
    {
        return _equipments;
    }
}

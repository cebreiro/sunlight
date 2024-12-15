#pragma once
#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include "sl/generator/game/debug/game_debug_type.h"
#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/item/item_remove_result.h"
#include "sl/generator/game/contents/item/equipment_position.h"
#include "sl/generator/game/contents/item/inventory_position.h"
#include "sl/generator/game/contents/item/item_position.h"
#include "sl/generator/game/contents/item/item_position_update.h"
#include "sl/generator/game/contents/item/item_slot_storage_base.h"
#include "sl/generator/game/contents/quick_slot/quick_slot_position.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/service/database/dto/character.h"
#include "sl/generator/service/database/transaction/item/item_log.h"
#include "sl/generator/service/gamedata/item/weapon_class_type.h"

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
        void ClearItemLog();

    public:
        bool IsValidVendorPage(int8_t page) const;

        bool IsEmpty(int8_t page, const ItemSlotRange& range) const;
        bool IsEquipped(EquipmentPosition position) const;

        bool HasInventoryItem(int32_t itemId, int32_t quantity) const;
        bool HasVendorSaleItem(int8_t page) const;

        auto GetVendorSaleItemSize() const -> int64_t;
        auto GetMixItemSize() const -> int64_t;

    public:
        void AddOrSubGold(int32_t value);
        void SetGold(int32_t value);

    public:
        bool AddInventoryItem(SharedPtrNotNull<GameItem> item, const InventoryPosition* hint = nullptr);
        bool AddQuickSlotItem(SharedPtrNotNull<GameItem> item, const QuickSlotPosition* hint = nullptr);

        bool AddVendorItem(SharedPtrNotNull<GameItem> item, int32_t slot);

        bool RemoveInventoryItemAll(int32_t itemId, std::vector<game_entity_id_type>* result);

        bool TryRemoveInventoryItem(int32_t itemId, int32_t quantity, std::vector<item_remove_result_type>* result);

        bool TryStackInventoryItem(int32_t itemId, int32_t quantity,
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

        bool LiftVendorItem(int8_t page);
        bool LowerPickedItemToVendor(int8_t page);
        bool SwapPickedItemToVendor(int8_t page);

        bool LowerPickedItemToMix();
        void MoveMixItemToInventory(std::vector<PtrNotNull<const GameItem>>& result);

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
        auto FindMixItemByItemId(int32_t itemId) const -> const GameItem*;
        auto FindEmptyInventoryPosition(int32_t width, int32_t height) const -> std::optional<InventoryPosition>;
        auto FindEmptyQuickSlotPosition() const -> std::optional<QuickSlotPosition>;

    public:
        auto GetGold() const -> int32_t;
        auto GetInventoryPage() const -> int32_t;
        auto GetPickedItem() const -> const GameItem*;
        auto GetEquipmentItem(EquipmentPosition position) const -> const GameItem*;
        auto GetVendorSaleItem(int32_t page) const -> const GameItem*;

        auto GetWeaponClass() const -> WeaponClassType;

        inline auto GetItemRange() const;
        inline auto GetEquipItems() const;
        inline auto GetVendorSaleItems() const;
        inline auto GetMixItems() const;

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
        void AddPosition(GameItem& item);
        void RemovePosition(GameItem& item);

        void SwapItemPosition(GameItem& first, GameItem& second);
        void UpdateItemPosition(GameItem& item, item_position_update_type updateType);

        void Insert(SharedPtrNotNull<GameItem> item);
        auto Erase(game_entity_id_type id) -> std::shared_ptr<GameItem>;

    private:
        int64_t _cid = 0;
        std::vector<db::ItemLog> _itemLogs;

        int32_t _gold = 0;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<GameItem>> _items;
        std::unordered_map<int64_t, PtrNotNull<GameItem>> _itemsUIdIndex;

        int32_t _inventoryPage = 0;
        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_INVENTORY_PAGE_SIZE> _inventorySlot;
        boost::unordered::unordered_flat_set<PtrNotNull<GameItem>> _itemStorageQueryResult;

        boost::container::flat_multimap<int32_t, PtrNotNull<GameItem>> _inventoryItemIdIndex;
        std::vector<PtrNotNull<GameItem>> _sortedStackableItemCache;

        std::array<GameItem*, static_cast<int32_t>(EquipmentPosition::Count)> _equipments = {};

        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_QUICK_SLOT_PAGE_SIZE> _quickSlotStorages;

        std::array<GameItem*, GameConstant::MAX_STREET_VENDOR_PAGE_SIZE> _vendorSaleItems = {};

        std::unordered_map<game_entity_id_type, PtrNotNull<GameItem>> _mixItems;
        
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

    inline auto PlayerItemComponent::GetVendorSaleItems() const
    {
        const auto filter = [](const auto& tuple) -> bool
            {
                return std::get<1>(tuple) != nullptr;
            };
        const auto transform = [](const auto& tuple) -> std::pair<int32_t, const GameItem&>
            {
                return std::pair<int32_t, const GameItem&>{ static_cast<int32_t>(std::get<0>(tuple)), *std::get<1>(tuple) };
            };

        return _vendorSaleItems | std::views::enumerate | std::views::filter(filter) | std::views::transform(transform);
    }

    inline auto PlayerItemComponent::GetMixItems() const
    {
        return _mixItems | std::views::values | notnull::reference;
    }
}

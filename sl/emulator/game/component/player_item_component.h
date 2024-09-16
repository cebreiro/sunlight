#pragma once
#include <boost/unordered/unordered_flat_set.hpp>
#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/game/contants/item/inventory_position.h"
#include "sl/emulator/game/contants/item/item_position.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/service/database/dto/character.h"
#include "sl/emulator/service/database/transaction/item/item_log.h"

namespace sunlight
{
    class GameEntityIdPublisher;
    class GameItem;
    class GameEntityIdPool;
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
        bool IsEquipped(EquipmentPosition position) const;

    public:
        bool AddInventoryItem(SharedPtrNotNull<GameItem> item, const InventoryPosition* hint = nullptr);
        bool TryStackOverlapItem(int32_t itemId, int32_t quantity, int32_t& usedQuantity);

    public:
        bool LiftEquipItem(EquipmentPosition position);
        bool LowerPickedItemTo(EquipmentPosition position);
        bool SwapPickedItemTo(EquipmentPosition position);

        bool LiftInventoryItem(game_entity_id_type itemId);
        bool LowerPickedItemTo(int8_t page, int8_t x, int8_t y);
        bool SwapPickedItemTo(int8_t page, int8_t x, int8_t y);

    public:
        auto FindEmptyInventoryPosition(int32_t width, int32_t height) const -> std::optional<InventoryPosition>;

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

    private:
        static auto ConvertToItemPosType(ItemPositionType position) -> db::ItemPosType;

        void AddItemUpdatePositionLog(const GameItem& item);
        void AddItemUpdateQuantityLog(const GameItem& item);
        void AddItemAddLog(const GameItem& item);

    private:
        int64_t _cid = 0;
        std::vector<db::ItemLog> _itemLogs;

        int32_t _gold = 0;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<GameItem>> _items;

        int32_t _inventoryPage = 0;
        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_INVENTORY_PAGE_SIZE> _inventorySlot;
        boost::unordered::unordered_flat_set<PtrNotNull<GameItem>> _inventorySlotQueryResult;

        std::array<GameItem*, static_cast<int32_t>(EquipmentPosition::Count)> _equipments = {};

        GameItem* _pickItem = nullptr;
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

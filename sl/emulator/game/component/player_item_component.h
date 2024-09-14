#pragma once
#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/item/equipment_position.h"
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/service/database/dto/character.h"

namespace sunlight
{
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
        PlayerItemComponent(GameEntityIdPool& idPool, const ItemDataProvider& itemDataProvider, const db::dto::Character& dto);
        ~PlayerItemComponent();

        auto GetGold() const -> int32_t;
        auto GetInventoryPage() const -> int32_t;
        auto GetPickedItem() const -> const GameItem*;

        inline auto GetItemRange() const;
        inline auto GetEquipItems() const;

    private:
        int32_t _gold = 0;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<GameItem>> _items;

        int32_t _inventoryPage = 0;
        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_INVENTORY_PAGE_SIZE> _inventory;

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

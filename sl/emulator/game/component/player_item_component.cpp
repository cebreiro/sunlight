#include "player_item_component.h"

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/contants/item/item_slot_stroage.h"
#include "sl/emulator/game/entity/game_entity_id_pool.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    PlayerItemComponent::PlayerItemComponent(GameEntityIdPool& idPool, const ItemDataProvider& itemDataProvider,
        const db::dto::Character& dto)
        : _gold(dto.gold)
        , _inventoryPage(std::clamp<int32_t>(dto.inventoryPage, 1, GameConstant::MAX_INVENTORY_PAGE_SIZE))
    {
        for (int64_t i = 0; i < std::ssize(_inventory); ++i)
        {
            _inventory[i] = std::make_unique<ItemSlotStorage>(GameConstant::INVENTORY_WIDTH, GameConstant::INVENTORY_HEIGHT);
        }

        for (const db::dto::Character::Item& dtoItem : dto.items)
        {
            const ItemData* data = itemDataProvider.Find(dtoItem.dataId);
            if (!data)
            {
                throw std::runtime_error(fmt::format("fail to find item data. cid: {}, item_id: {}",
                    dto.id, dtoItem.dataId));
            }

            const game_entity_id_type itemEntityId = idPool.Pop(GameEntityType::Item);

            SharedPtrNotNull<GameItem>& itemEntity = _items[itemEntityId];
            assert(!itemEntity);

            itemEntity = std::make_shared<GameItem>(itemEntityId, *data, dtoItem.quantity);
            itemEntity->SetUId(dtoItem.id);

            auto positionComponent = new ItemPositionComponent();
            positionComponent->SetPage(dtoItem.page);
            positionComponent->SetX(dtoItem.x);
            positionComponent->SetY(dtoItem.y);

            (void)itemEntity->AddComponent(std::unique_ptr<ItemPositionComponent>(positionComponent));

            switch (dtoItem.posType)
            {
            case db::ItemPosType::Inventory:
            {
                positionComponent->SetPositionType(ItemPositionType::Inventory);

                if (positionComponent->GetPage() >= std::ssize(_inventory))
                {
                    assert(false);

                    continue;
                }

                ItemSlotStorage& storage = *_inventory[positionComponent->GetPage()];

                const ItemSlotRange range{
                    .x = positionComponent->GetX(),
                    .y = positionComponent->GetY(),
                    .xSize = itemEntity->GetData().GetWidth(),
                    .ySize = itemEntity->GetData().GetHeight(),
                };

                if (!storage.Contains(range))
                {
                    throw std::runtime_error(fmt::format("invalid item slot. cid: {}, item_id: {}, range: [X: {}, y: {}, x_size: {}, y_size: {}]",
                        dto.id, dtoItem.id, range.x, range.y, range.xSize, range.ySize));
                }

                if (!storage.HasEmptySlot(range))
                {
                    throw std::runtime_error(fmt::format("item overlapped. cid: {}, item_id: {}, range: [X: {}, y: {}, x_size: {}, y_size: {}]",
                        dto.id, dtoItem.id, range.x, range.y, range.xSize, range.ySize));
                }

                storage.Set(itemEntity.get(), range);
            }
            break;
            case db::ItemPosType::Equipment:
            {
                positionComponent->SetPositionType(ItemPositionType::Equipment);

                const int64_t index = positionComponent->GetPage();
                if (index < 0 || index >= std::ssize(_equipments))
                {
                    throw std::runtime_error(fmt::format("invalid equipment slot. cid: {}, item_id: {}, slot: {}",
                        dto.id, dtoItem.id, index));
                }

                GameItem*& equipment = _equipments[index];
                if (equipment)
                {
                    throw std::runtime_error(fmt::format("duplicated equipment item. cid: {}, item_id: {}, slot: {}",
                        dto.id, dtoItem.id, index));
                }

                equipment = itemEntity.get();
            }
            break;
            case db::ItemPosType::QuickSlot:
            {
                positionComponent->SetPositionType(ItemPositionType::QuickSlot);
            }
            break;
            case db::ItemPosType::Pick:
            {
                positionComponent->SetPositionType(ItemPositionType::Pick);

                if (_pickItem)
                {
                    throw std::runtime_error(fmt::format("duplicated pick item. cid: {}, item_id: {}, already_picked_item_id: {}",
                        dto.id, dtoItem.id, _pickItem->GetUId().value_or(-1)));
                }

                _pickItem = itemEntity.get();
            }
            break;
            }
        }
    }

    PlayerItemComponent::~PlayerItemComponent()
    {
    }

    bool PlayerItemComponent::IsEquipped(EquipmentPosition position) const
    {
        return GetEquipmentItem(position) != nullptr;
    }

    bool PlayerItemComponent::LiftEquipItem(EquipmentPosition position)
    {
        if (_pickItem)
        {
            return false;
        }

        GameItem*& equipItem = Mutable(position);
        if (!equipItem)
        {
            return false;
        }

        ItemPositionComponent& itemPositionComponent = equipItem->GetComponent<ItemPositionComponent>();
        itemPositionComponent.SetPositionType(ItemPositionType::Pick);
        itemPositionComponent.ResetPosition();

        std::swap(_pickItem, equipItem);

        return true;
    }

    bool PlayerItemComponent::LowerPickedItemTo(EquipmentPosition position)
    {
        if (!_pickItem)
        {
            return false;
        }

        if (IsEquipped(position))
        {
            return false;
        }

        ItemPositionComponent& itemPositionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        itemPositionComponent.SetPositionType(ItemPositionType::Equipment);
        itemPositionComponent.SetPage(static_cast<int8_t>(position));

        std::swap(_pickItem, Mutable(position));

        return true;
    }

    bool PlayerItemComponent::SwapPickedItemTo(EquipmentPosition position)
    {
        if (!_pickItem)
        {
            return false;
        }

        GameItem*& equipItem = Mutable(position);
        if (!equipItem)
        {
            return false;
        }

        _pickItem->GetComponent<ItemPositionComponent>().SwapPosition(
            equipItem->GetComponent<ItemPositionComponent>());

        std::swap(_pickItem, equipItem);

        return true;
    }

    bool PlayerItemComponent::LiftInventoryItem(game_entity_id_type itemId)
    {
        if (_pickItem)
        {
            return false;
        }

        const auto iter = _items.find(itemId);
        if (iter == _items.end())
        {
            return false;
        }

        GameItem* item = iter->second.get();
        assert(item);

        ItemPositionComponent& itemPositionComponent = item->GetComponent<ItemPositionComponent>();
        if (itemPositionComponent.GetPositionType() != ItemPositionType::Inventory)
        {
            return false;
        }

        const ItemSlotRange slotRange{
            .x = itemPositionComponent.GetX(),
            .y = itemPositionComponent.GetY(),
            .xSize = item->GetData().GetWidth(),
            .ySize = item->GetData().GetHeight(),
        };

        GetInventorySlotStorage(itemPositionComponent.GetPage())->Set(nullptr, slotRange);

        itemPositionComponent.SetPositionType(ItemPositionType::Pick);
        itemPositionComponent.ResetPosition();

        _pickItem = item;

        return true;
    }

    bool PlayerItemComponent::LowerPickedItemTo(int8_t page, int8_t x, int8_t y)
    {
        if (!_pickItem)
        {
            return false;
        }

        ItemSlotStorage* storage = GetInventorySlotStorage(page);
        if (!storage)
        {
            return false;
        }

        const ItemSlotRange slotRange{
            .x = x,
            .y = y,
            .xSize = _pickItem->GetData().GetWidth(),
            .ySize = _pickItem->GetData().GetHeight(),
        };

        if (!storage->Contains(slotRange))
        {
            return false;
        }

        _inventoryQueryResult.clear();
        storage->Get(_inventoryQueryResult, slotRange);

        if (!_inventoryQueryResult.empty())
        {
            return false;
        }

        ItemPositionComponent& itemPositionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        itemPositionComponent.SetPositionType(ItemPositionType::Inventory);
        itemPositionComponent.SetPosition(page, static_cast<int8_t>(slotRange.x), static_cast<int8_t>(slotRange.y));

        storage->Set(_pickItem, slotRange);
        _pickItem = nullptr;

        return true;
    }

    bool PlayerItemComponent::SwapPickedItemTo(int8_t page, int8_t x, int8_t y)
    {
        if (!_pickItem)
        {
            return false;
        }

        ItemSlotStorage* storage = GetInventorySlotStorage(page);
        if (!storage)
        {
            return false;
        }

        const ItemSlotRange slotRange{
            .x = x,
            .y = y,
            .xSize = _pickItem->GetData().GetWidth(),
            .ySize = _pickItem->GetData().GetHeight(),
        };

        if (!storage->Contains(slotRange))
        {
            return false;
        }

        _inventoryQueryResult.clear();
        storage->Get(_inventoryQueryResult, slotRange);

        if (_inventoryQueryResult.size() != 1)
        {
            return false;
        }

        GameItem* inventoryItem = *_inventoryQueryResult.begin();
        ItemPositionComponent& inventoryItemPositionComponent = inventoryItem->GetComponent<ItemPositionComponent>();

        storage->Set(nullptr, ItemSlotRange{
            .x = inventoryItemPositionComponent.GetX(),
            .y = inventoryItemPositionComponent.GetY(),
            .xSize = inventoryItem->GetData().GetWidth(),
            .ySize = inventoryItem->GetData().GetHeight(),
            });
        storage->Set(_pickItem, slotRange);

        _pickItem->GetComponent<ItemPositionComponent>().SwapPosition(inventoryItemPositionComponent);
        _pickItem = inventoryItem;

        return true;
    }

    auto PlayerItemComponent::GetGold() const -> int32_t
    {
        return _gold;
    }

    auto PlayerItemComponent::GetInventoryPage() const -> int32_t
    {
        return _inventoryPage;
    }

    auto PlayerItemComponent::GetPickedItem() const -> const GameItem*
    {
        return _pickItem;
    }

    auto PlayerItemComponent::GetEquipmentItem(EquipmentPosition position) const -> const GameItem*
    {
        return _equipments[static_cast<int32_t>(position)];
    }

    auto PlayerItemComponent::Mutable(EquipmentPosition position) -> GameItem*&
    {
        return _equipments[static_cast<int32_t>(position)];
    }

    auto PlayerItemComponent::GetInventorySlotStorage(int8_t page) -> ItemSlotStorage*
    {
        if (page < 0 || page >= std::ssize(_inventory))
        {
            return nullptr;
        }

        return _inventory[page].get();
    }
}

#include "player_item_component.h"

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/contants/item/item_slot_stroage.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    PlayerItemComponent::PlayerItemComponent(GameEntityIdPublisher& idPublisher, const ItemDataProvider& itemDataProvider,
        const db::dto::Character& dto)
        : _cid(dto.id)
        , _gold(dto.gold)
        , _inventoryPage(std::clamp<int32_t>(dto.inventoryPage, 1, GameConstant::MAX_INVENTORY_PAGE_SIZE))
    {
        for (int64_t i = 0; i < std::ssize(_inventorySlot); ++i)
        {
            _inventorySlot[i] = std::make_unique<ItemSlotStorage>(GameConstant::INVENTORY_WIDTH, GameConstant::INVENTORY_HEIGHT);
        }

        for (const db::dto::Character::Item& dtoItem : dto.items)
        {
            const ItemData* data = itemDataProvider.Find(dtoItem.dataId);
            if (!data)
            {
                throw std::runtime_error(fmt::format("fail to find item data. cid: {}, item_id: {}",
                    dto.id, dtoItem.dataId));
            }

            auto itemEntity = std::make_shared<GameItem>(idPublisher, *data, dtoItem.quantity);
            _items[itemEntity->GetId()] = itemEntity;

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

                if (positionComponent->GetPage() >= std::ssize(_inventorySlot))
                {
                    assert(false);

                    continue;
                }

                ItemSlotStorage& storage = *_inventorySlot[positionComponent->GetPage()];

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

    bool PlayerItemComponent::HasItemLog() const
    {
        return !_itemLogs.empty();
    }

    void PlayerItemComponent::FlushItemLogTo(std::vector<db::ItemLog>& dest)
    {
        if (_itemLogs.empty())
        {
            return;
        }

        dest.insert_range(dest.end(), _itemLogs);
        _itemLogs.clear();
    }

    bool PlayerItemComponent::IsEquipped(EquipmentPosition position) const
    {
        return GetEquipmentItem(position) != nullptr;
    }

    bool PlayerItemComponent::AddInventoryItem(SharedPtrNotNull<GameItem> item, const InventoryPosition* hint)
    {
        assert(item->GetUId().has_value());
        assert(item->FindComponent<ItemPositionComponent>());

        const ItemData& itemData = item->GetData();
        const int32_t width = itemData.GetWidth();
        const int32_t height = itemData.GetHeight();

        const auto position = [&]() -> std::optional<InventoryPosition>
            {
                if (hint)
                {
                    return *hint;
                }

                return FindEmptyInventoryPosition(width, height);
            }();

        if (!position.has_value())
        {
            return false;
        }

        ItemPositionComponent& positionComponent = item->GetComponent<ItemPositionComponent>();
        positionComponent.SetPositionType(ItemPositionType::Inventory);
        positionComponent.SetPage(position->page);
        positionComponent.SetX(position->x);
        positionComponent.SetY(position->y);

        ItemSlotStorage* slotStorage = GetInventorySlotStorage(position->page);
        assert(slotStorage);

        const ItemSlotRange range{
            .x = position->x,
            .y = position->y,
            .xSize = width,
            .ySize = height
        };

        assert(slotStorage->HasEmptySlot(range));

        slotStorage->Set(item.get(), range);

        AddItemAddLog(*item);

        const game_entity_id_type id = item->GetId();

        [[maybe_unused]]
        const bool inserted = _items.try_emplace(id, std::move(item)).second;
        assert(inserted);

        return true;
    }

    bool PlayerItemComponent::TryStackOverlapItem(int32_t itemId, int32_t quantity, int32_t& usedQuantity)
    {
        if (quantity <= 0)
        {
            assert(false);

            return false;
        }

        int32_t remainQuantity = quantity;

        for (GameItem& item : _items | std::views::values | notnull::reference)
        {
            if (item.GetData().GetId() != itemId)
            {
                continue;
            }

            const int32_t maxOverlapCount = item.GetData().GetMaxOverlapCount();
            if (maxOverlapCount <= 1 || item.GetQuantity() >= maxOverlapCount)
            {
                continue;
            }

            const int32_t possibleCount = maxOverlapCount - item.GetQuantity();
            const int32_t addQuantity = possibleCount > remainQuantity ? remainQuantity : possibleCount;

            remainQuantity -= addQuantity;
            assert(remainQuantity >= 0);

            item.SetQuantity(item.GetQuantity() + addQuantity);
            assert(item.GetQuantity() <= maxOverlapCount);

            AddItemUpdateQuantityLog(item);

            if (remainQuantity <= 0)
            {
                break;
            }
        }

        usedQuantity = (quantity - remainQuantity);

        return remainQuantity != quantity;
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

        AddItemUpdatePositionLog(*_pickItem);

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

        AddItemUpdatePositionLog(*_pickItem);

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

        AddItemUpdatePositionLog(*_pickItem);
        AddItemUpdatePositionLog(*equipItem);

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

        AddItemUpdatePositionLog(*_pickItem);

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

        _inventorySlotQueryResult.clear();
        storage->Get(_inventorySlotQueryResult, slotRange);

        if (!_inventorySlotQueryResult.empty())
        {
            return false;
        }

        ItemPositionComponent& itemPositionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        itemPositionComponent.SetPositionType(ItemPositionType::Inventory);
        itemPositionComponent.SetPosition(page, static_cast<int8_t>(slotRange.x), static_cast<int8_t>(slotRange.y));

        storage->Set(_pickItem, slotRange);

        AddItemUpdatePositionLog(*_pickItem);

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

        _inventorySlotQueryResult.clear();
        storage->Get(_inventorySlotQueryResult, slotRange);

        if (_inventorySlotQueryResult.size() != 1)
        {
            return false;
        }

        GameItem* inventoryItem = *_inventorySlotQueryResult.begin();
        ItemPositionComponent& inventoryItemPositionComponent = inventoryItem->GetComponent<ItemPositionComponent>();

        storage->Set(nullptr, ItemSlotRange{
            .x = inventoryItemPositionComponent.GetX(),
            .y = inventoryItemPositionComponent.GetY(),
            .xSize = inventoryItem->GetData().GetWidth(),
            .ySize = inventoryItem->GetData().GetHeight(),
            });
        storage->Set(_pickItem, slotRange);

        _pickItem->GetComponent<ItemPositionComponent>().SwapPosition(inventoryItemPositionComponent);

        AddItemUpdatePositionLog(*_pickItem);
        AddItemUpdatePositionLog(*inventoryItem);

        _pickItem = inventoryItem;

        return true;
    }

    auto PlayerItemComponent::FindEmptyInventoryPosition(int32_t width, int32_t height) const -> std::optional<InventoryPosition>
    {
        for (int32_t i = 0; i < _inventoryPage; ++i)
        {
            ItemSlotStorage& inventory = *_inventorySlot[i];

            if (const std::optional<std::pair<int32_t, int32_t>> pos = inventory.FindEmpty(width, height);
                pos)
            {
                assert(inventory.HasEmptySlot(ItemSlotRange{
                    .x = pos->first,
                    .y = pos->second,
                    .xSize = width,
                    .ySize = height,
                    }));

                return InventoryPosition{
                    .page = static_cast<int8_t>(i),
                    .x = static_cast<int8_t>(pos->first),
                    .y = static_cast<int8_t>(pos->second),
                };
            }
        }

        return std::nullopt;
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
        if (page < 0 || page >= std::ssize(_inventorySlot))
        {
            return nullptr;
        }

        return _inventorySlot[page].get();
    }

    auto PlayerItemComponent::ConvertToItemPosType(ItemPositionType position) -> db::ItemPosType
    {
        switch (position)
        {
        case ItemPositionType::Inventory:
            return db::ItemPosType::Inventory;
        case ItemPositionType::Equipment:
            return db::ItemPosType::Equipment;
        case ItemPositionType::QuickSlot:
            return db::ItemPosType::QuickSlot;
        case ItemPositionType::Pick:
            return db::ItemPosType::Pick;
        default:;
        }

        assert(false);

        return db::ItemPosType::Inventory;
    }

    void PlayerItemComponent::AddItemUpdatePositionLog(const GameItem& item)
    {
        assert(item.GetUId().has_value());

        const ItemPositionComponent& itemPositionComponent = item.GetComponent<ItemPositionComponent>();

        _itemLogs.emplace_back(db::ItemLogUpdatePosition{
            .id = item.GetUId().value(),
            .posType = ConvertToItemPosType(itemPositionComponent.GetPositionType()),
            .page = itemPositionComponent.GetPage(),
            .x = itemPositionComponent.GetX(),
            .y = itemPositionComponent.GetY()
        });
    }

    void PlayerItemComponent::AddItemUpdateQuantityLog(const GameItem& item)
    {
        _itemLogs.emplace_back(db::ItemLogUpdateQuantity{
            .id = item.GetUId().value(),
            .quantity = item.GetQuantity(),
            });
    }

    void PlayerItemComponent::AddItemAddLog(const GameItem& item)
    {
        const ItemPositionComponent& itemPositionComponent = item.GetComponent<ItemPositionComponent>();

        _itemLogs.emplace_back(db::ItemLogAdd{
            .id = item.GetUId().value(),
            .cid = _cid,
            .itemId = item.GetData().GetId(),
            .quantity = item.GetQuantity(),
            .posType = ConvertToItemPosType(itemPositionComponent.GetPositionType()),
            .page = itemPositionComponent.GetPage(),
            .x = itemPositionComponent.GetX(),
            .y = itemPositionComponent.GetY()
            });
    }
}

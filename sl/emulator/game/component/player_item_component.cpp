#include "player_item_component.h"

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/contants/item/item_slot_stroage.h"
#include "sl/emulator/game/debug/game_debugger.h"
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

        for (int64_t i = 0; i < std::ssize(_quickSlotStorages); ++i)
        {
            _quickSlotStorages[i] = std::make_unique<ItemSlotStorage>(GameConstant::QUICK_SLOT_WIDTH, GameConstant::QUICK_SLOT_HEIGHT);
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

                if (positionComponent->GetPage() < 0 || positionComponent->GetPage() >= std::ssize(_inventorySlot))
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

                if (positionComponent->GetPage() < 0 || positionComponent->GetPage() >= std::ssize(_quickSlotStorages))
                {
                    assert(false);

                    continue;
                }

                ItemSlotStorage& storage = *_quickSlotStorages[positionComponent->GetPage()];

                const ItemSlotRange range{
                    .x = positionComponent->GetX(),
                    .y = positionComponent->GetY(),
                    .xSize = 1,
                    .ySize = 1,
                };

                if (!storage.Contains(range))
                {
                    throw std::runtime_error(fmt::format("invalid quick slot. cid: {}, item_id: {}, range: [X: {}, y: {}]",
                        dto.id, dtoItem.id, range.x, range.y));
                }

                if (!storage.HasEmptySlot(range))
                {
                    throw std::runtime_error(fmt::format("quick slot overlapped. cid: {}, item_id: {}, range: [X: {}, y: {}]",
                        dto.id, dtoItem.id, range.x, range.y));
                }

                storage.Set(itemEntity.get(), range);
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
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, slotStorage->GetDebugString());

        AddItemAddLog(*item);

        const game_entity_id_type id = item->GetId();

        [[maybe_unused]]
        const bool inserted = _items.try_emplace(id, std::move(item)).second;
        assert(inserted);

        return true;
    }

    bool PlayerItemComponent::AddQuickSlotItem(SharedPtrNotNull<GameItem> item, const QuickSlotPosition* hint)
    {
        assert(item->GetUId().has_value());
        assert(item->FindComponent<ItemPositionComponent>());

        const auto position = [&]() -> std::optional<QuickSlotPosition>
            {
                if (hint)
                {
                    return *hint;
                }

                return FindEmptyQuickSlotPosition();
            }();

        if (!position.has_value())
        {
            return false;
        }

        ItemPositionComponent& positionComponent = item->GetComponent<ItemPositionComponent>();
        positionComponent.SetPositionType(ItemPositionType::QuickSlot);
        positionComponent.SetPage(position->page);
        positionComponent.SetX(position->x);
        positionComponent.SetY(position->y);

        ItemSlotStorage* slotStorage = GetQuickSlotStorage(position->page);
        assert(slotStorage);

        const ItemSlotRange range{
            .x = position->x,
            .y = position->y,
            .xSize = 1,
            .ySize = 1
        };

        assert(slotStorage->HasEmptySlot(range));

        slotStorage->Set(item.get(), range);
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, slotStorage->GetDebugString());

        AddItemAddLog(*item);

        const game_entity_id_type id = item->GetId();

        [[maybe_unused]]
        const bool inserted = _items.try_emplace(id, std::move(item)).second;
        assert(inserted);

        return true;
    }

    bool PlayerItemComponent::TryStackItem(int32_t itemId, int32_t quantity, int32_t& usedQuantity,
        std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>>* result)
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

            if (result)
            {
                result->emplace_back(&item, addQuantity);
            }

            if (remainQuantity <= 0)
            {
                break;
            }
        }

        usedQuantity = (quantity - remainQuantity);

        return remainQuantity != quantity;
    }

    bool PlayerItemComponent::TryStackQuickSlotItem(const ItemData& itemData, int32_t quantity, int32_t& usedQuantity,
        std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>>* result)
    {
        if (quantity <= 0)
        {
            assert(false);

            return false;
        }

        if (!itemData.IsAbleToUseQuickSlot())
        {
            return false;
        }

        int32_t remainQuantity = quantity;

        for (GameItem& item : _items | std::views::values | notnull::reference)
        {
            if (item.GetData().GetId() != itemData.GetId())
            {
                continue;
            }

            if (!item.GetComponent<ItemPositionComponent>().IsInQuickSlot())
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

            if (result)
            {
                result->emplace_back(&item, addQuantity);
            }

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

    bool PlayerItemComponent::LiftItem(game_entity_id_type itemId)
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

        ItemPositionComponent& positionComponent = iter->second->GetComponent<ItemPositionComponent>();
        switch (positionComponent.GetPositionType())
        {
        case ItemPositionType::Inventory:
        {
            GetInventorySlotStorage(positionComponent.GetPage())->Set(nullptr,
                ItemSlotRange{
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
                .xSize = iter->second->GetData().GetWidth(),
                .ySize = iter->second->GetData().GetHeight(),
                });
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, GetInventorySlotStorage(positionComponent.GetPage())->GetDebugString());
        }
        break;
        case ItemPositionType::Equipment:
        {
            GameItem*& equipItem = Mutable(static_cast<EquipmentPosition>(positionComponent.GetPage()));
            assert(equipItem && equipItem == iter->second.get());

            equipItem = nullptr;
        }
        break;
        case ItemPositionType::Pick:
        {
            return false;
        }
        case ItemPositionType::QuickSlot:
        {
            GetQuickSlotStorage(positionComponent.GetPage())->Set(nullptr,
                ItemSlotRange{
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
                .xSize = 1,
                .ySize = 1,
                });
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, GetQuickSlotStorage(positionComponent.GetPage())->GetDebugString());
        }
        break;
        case ItemPositionType::Count:
        default:
            assert(false);
        }

        positionComponent.SetPositionType(ItemPositionType::Pick);
        positionComponent.ResetPosition();

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

        _itemStorageQueryResult.clear();
        storage->Get(_itemStorageQueryResult, slotRange);

        if (!_itemStorageQueryResult.empty())
        {
            return false;
        }

        ItemPositionComponent& itemPositionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        itemPositionComponent.SetPositionType(ItemPositionType::Inventory);
        itemPositionComponent.SetPosition(page, static_cast<int8_t>(slotRange.x), static_cast<int8_t>(slotRange.y));

        storage->Set(_pickItem, slotRange);
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, storage->GetDebugString());

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

        _itemStorageQueryResult.clear();
        storage->Get(_itemStorageQueryResult, slotRange);

        if (_itemStorageQueryResult.size() != 1)
        {
            return false;
        }

        GameItem* inventoryItem = *_itemStorageQueryResult.begin();
        ItemPositionComponent& positionComponent = inventoryItem->GetComponent<ItemPositionComponent>();

        storage->Set(nullptr, ItemSlotRange{
            .x = positionComponent.GetX(),
            .y = positionComponent.GetY(),
            .xSize = inventoryItem->GetData().GetWidth(),
            .ySize = inventoryItem->GetData().GetHeight(),
            });
        storage->Set(_pickItem, slotRange);
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, storage->GetDebugString());

        positionComponent.SetPositionType(ItemPositionType::Pick);
        positionComponent.ResetPosition();

        ItemPositionComponent& pickItemPositionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        pickItemPositionComponent.SetPositionType(ItemPositionType::Inventory);
        pickItemPositionComponent.SetPosition(page, x, y);

        AddItemUpdatePositionLog(*_pickItem);
        AddItemUpdatePositionLog(*inventoryItem);

        _pickItem = inventoryItem;

        return true;
    }

    bool PlayerItemComponent::LowerPickedItemToQuickSlot(int8_t page, int8_t x, int8_t y)
    {
        if (!_pickItem)
        {
            return false;
        }

        ItemSlotStorage* storage = GetQuickSlotStorage(page);
        if (!storage)
        {
            return false;
        }

        const ItemSlotRange slotRange{
            .x = x,
            .y = y,
            .xSize = 1,
            .ySize = 1,
        };

        if (!storage->Contains(slotRange))
        {
            return false;
        }

        _itemStorageQueryResult.clear();
        storage->Get(_itemStorageQueryResult, slotRange);

        if (!_itemStorageQueryResult.empty())
        {
            return false;
        }

        ItemPositionComponent& itemPositionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        itemPositionComponent.SetPositionType(ItemPositionType::QuickSlot);
        itemPositionComponent.SetPosition(page, static_cast<int8_t>(slotRange.x), static_cast<int8_t>(slotRange.y));

        storage->Set(_pickItem, slotRange);
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, storage->GetDebugString());

        AddItemUpdatePositionLog(*_pickItem);

        _pickItem = nullptr;

        return true;
    }

    bool PlayerItemComponent::SwapPickedItemToQuickSlot(int8_t page, int8_t x, int8_t y)
    {
        if (!_pickItem)
        {
            return false;
        }

        ItemSlotStorage* storage = GetQuickSlotStorage(page);
        if (!storage)
        {
            return false;
        }

        const ItemSlotRange slotRange{
            .x = x,
            .y = y,
            .xSize = 1,
            .ySize = 1,
        };

        if (!storage->Contains(slotRange))
        {
            return false;
        }

        _itemStorageQueryResult.clear();
        storage->Get(_itemStorageQueryResult, slotRange);

        if (_itemStorageQueryResult.size() != 1)
        {
            return false;
        }

        GameItem* inventoryItem = *_itemStorageQueryResult.begin();
        ItemPositionComponent& positionComponent = inventoryItem->GetComponent<ItemPositionComponent>();

        storage->Set(nullptr, ItemSlotRange{
            .x = positionComponent.GetX(),
            .y = positionComponent.GetY(),
            .xSize = 1,
            .ySize = 1,
            });
        storage->Set(_pickItem, slotRange);
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, storage->GetDebugString());

        positionComponent.SetPositionType(ItemPositionType::Pick);
        positionComponent.ResetPosition();

        ItemPositionComponent& pickItemPositionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        pickItemPositionComponent.SetPositionType(ItemPositionType::QuickSlot);
        pickItemPositionComponent.SetPosition(page, x, y);

        AddItemUpdatePositionLog(*_pickItem);
        AddItemUpdatePositionLog(*inventoryItem);

        _pickItem = inventoryItem;

        return true;
    }

    bool PlayerItemComponent::SwapWeaponItem()
    {
        GameItem*& mainWeapon = Mutable(EquipmentPosition::Weapon1);
        GameItem*& subWeapon = Mutable(EquipmentPosition::Weapon2);

        if (!mainWeapon && !subWeapon)
        {
            return false;
        }

        if (mainWeapon)
        {
            mainWeapon->GetComponent<ItemPositionComponent>().SetPage(static_cast<int8_t>(EquipmentPosition::Weapon2));
            AddItemUpdatePositionLog(*mainWeapon);
        }

        if (subWeapon)
        {
            subWeapon->GetComponent<ItemPositionComponent>().SetPage(static_cast<int8_t>(EquipmentPosition::Weapon1));
            AddItemUpdatePositionLog(*subWeapon);
        }

        std::swap(mainWeapon, subWeapon);

        return true;
    }

    bool PlayerItemComponent::SetItemQuantity(game_entity_id_type id, int32_t quantity)
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return false;
        }

        if (iter->second->GetQuantity() != quantity)
        {
            iter->second->SetQuantity(quantity);

            AddItemUpdateQuantityLog(*iter->second);
        }

        return true;
    }

    bool PlayerItemComponent::IncreaseItemQuantity(game_entity_id_type id, int32_t quantity)
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return false;
        }

        iter->second->SetQuantity(iter->second->GetQuantity() + quantity);

        AddItemUpdateQuantityLog(*iter->second);

        return true;
    }

    bool PlayerItemComponent::DecreaseItemQuantity(game_entity_id_type id, int32_t quantity)
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return false;
        }

        const int32_t itemQuantity = iter->second->GetQuantity();
        if (itemQuantity <= quantity)
        {
            return false;
        }

        iter->second->SetQuantity(itemQuantity - quantity);
        assert(iter->second->GetQuantity() > 0);

        AddItemUpdateQuantityLog(*iter->second);

        return true;
    }

    bool PlayerItemComponent::AddNewPickedItem(SharedPtrNotNull<GameItem> item)
    {
        assert(item->GetUId().has_value());
        assert(item->HasComponent<ItemPositionComponent>());
        assert(!_items.contains(item->GetId()));

        if (_pickItem)
        {
            return false;
        }

        _pickItem = item.get();
        _items[_pickItem->GetId()] = std::move(item);

        ItemPositionComponent& positionComponent = _pickItem->GetComponent<ItemPositionComponent>();
        positionComponent.SetPositionType(ItemPositionType::Pick);
        positionComponent.ResetPosition();

        AddItemAddLog(*_pickItem);

        return true;
    }

    bool PlayerItemComponent::RemoveItem(game_entity_id_type id)
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return false;
        }

        ItemPositionComponent& positionComponent = iter->second->GetComponent<ItemPositionComponent>();
        switch (positionComponent.GetPositionType())
        {
        case ItemPositionType::Inventory:
        {
            GetInventorySlotStorage(positionComponent.GetPage())->Set(nullptr,
                ItemSlotRange{
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
                .xSize = iter->second->GetData().GetWidth(),
                .ySize = iter->second->GetData().GetHeight(),
            });

            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, GetInventorySlotStorage(positionComponent.GetPage())->GetDebugString());
        }
        break;
        case ItemPositionType::Equipment:
        {
            GameItem*& equipItem = Mutable(static_cast<EquipmentPosition>(positionComponent.GetPage()));
            assert(equipItem && equipItem == iter->second.get());

            equipItem = nullptr;
        }
        break;
        case ItemPositionType::Pick:
        {
            assert(_pickItem == iter->second.get());

            _pickItem = nullptr;
        }
        break;
        case ItemPositionType::QuickSlot:
        {
            GetQuickSlotStorage(positionComponent.GetPage())->Set(nullptr,
                ItemSlotRange{
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
                .xSize = 1,
                .ySize = 1,
                });
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, GetQuickSlotStorage(positionComponent.GetPage())->GetDebugString());
        }
        break;
        case ItemPositionType::Count:
        default:
            assert(false);
        }

        AddItemRemoveLog(*iter->second);

        _items.erase(iter);

        return true;
    }

    auto PlayerItemComponent::ReleaseItem(game_entity_id_type id) -> SharedPtrNotNull<GameItem>
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return {};
        }

        ItemPositionComponent& positionComponent = iter->second->GetComponent<ItemPositionComponent>();
        switch (positionComponent.GetPositionType())
        {
        case ItemPositionType::Inventory:
        {
            GetInventorySlotStorage(positionComponent.GetPage())->Set(nullptr,
                ItemSlotRange{
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
                .xSize = iter->second->GetData().GetWidth(),
                .ySize = iter->second->GetData().GetHeight(),
                });
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, GetInventorySlotStorage(positionComponent.GetPage())->GetDebugString());
        }
        break;
        case ItemPositionType::Equipment:
        {
            GameItem*& equipItem = Mutable(static_cast<EquipmentPosition>(positionComponent.GetPage()));
            assert(equipItem && equipItem == iter->second.get());

            equipItem = nullptr;
        }
        break;
        case ItemPositionType::Pick:
        {
            assert(_pickItem == iter->second.get());

            _pickItem = nullptr;
        }
        break;
        case ItemPositionType::QuickSlot:
        {
            GetQuickSlotStorage(positionComponent.GetPage())->Set(nullptr,
                ItemSlotRange{
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
                .xSize = 1,
                .ySize = 1,
                });
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, GetQuickSlotStorage(positionComponent.GetPage())->GetDebugString());
        }
        break;
        case ItemPositionType::Count:
        default:
            assert(false);
        }

        AddItemRemoveLog(*iter->second);

        SharedPtrNotNull<GameItem> result = std::move(iter->second);

        _items.erase(iter);

        return result;
    }

    auto PlayerItemComponent::FindItem(game_entity_id_type id) const -> const GameItem*
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return nullptr;
        }

        return iter->second.get();
    }

    auto PlayerItemComponent::FindEquipmentItem(EquipmentPosition position) const -> const GameItem*
    {
        const int64_t index = static_cast<int64_t>(position);
        assert(index >= 0 && index < std::ssize(_equipments));

        return _equipments[index];
    }

    auto PlayerItemComponent::FindInventoryItem(game_entity_id_type id) const -> const GameItem*
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return nullptr;
        }

        const ItemPositionComponent& positionComponent = iter->second->GetComponent<ItemPositionComponent>();
        if (positionComponent.GetPositionType() != ItemPositionType::Inventory)
        {
            return nullptr;
        }

        return iter->second.get();
    }

    auto PlayerItemComponent::FindQuickSlotItem(game_entity_id_type id) const -> const GameItem*
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return nullptr;
        }

        const ItemPositionComponent& positionComponent = iter->second->GetComponent<ItemPositionComponent>();
        if (positionComponent.GetPositionType() != ItemPositionType::QuickSlot)
        {
            return nullptr;
        }

        return iter->second.get();
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

    auto PlayerItemComponent::FindEmptyQuickSlotPosition() const -> std::optional<QuickSlotPosition>
    {
        for (int64_t i = 0; i < std::ssize(_quickSlotStorages); ++i)
        {
            constexpr int32_t size = 1;

            if (const auto pos = _quickSlotStorages[i]->FindEmpty(size, size); pos)
            {
                return QuickSlotPosition{
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

    auto PlayerItemComponent::GetQuickSlotStorage(int8_t page) -> ItemSlotStorage*
    {
        if (page < 0 || page >= std::ssize(_quickSlotStorages))
        {
            return nullptr;
        }

        return _quickSlotStorages[page].get();
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

    void PlayerItemComponent::AddItemRemoveLog(const GameItem& item)
    {
        _itemLogs.emplace_back(db::ItemLogRemove{
            .id = item.GetUId().value(),
            });
    }
}

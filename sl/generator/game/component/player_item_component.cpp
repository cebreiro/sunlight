#include "player_item_component.h"

#include "sl/generator/game/component/item_position_component.h"
#include "sl/generator/game/contents/item/item_slot_stroage.h"
#include "sl/generator/game/data/sox/item_weapon.h"
#include "sl/generator/game/debug/game_debugger.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"

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
            _itemsUIdIndex[dtoItem.id] = itemEntity.get();

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
                _inventoryItemIdIndex.emplace(dtoItem.dataId, itemEntity.get());
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
            case db::ItemPosType::Vendor:
            {
                positionComponent->SetPositionType(ItemPositionType::Vendor);

                const int32_t index = positionComponent->GetPage();

                if (index < 0 || index >= std::ssize(_vendorSaleItems))
                {
                    assert(false);

                    continue;
                }

                GameItem*& vendorSaleItem = _vendorSaleItems[index];
                if (vendorSaleItem)
                {
                    assert(false);

                    continue;
                }

                vendorSaleItem = itemEntity.get();
            }
            break;
            case db::ItemPosType::Mix:
            {
                positionComponent->SetPositionType(ItemPositionType::Mix);

                _mixItems[itemEntity->GetId()] = itemEntity.get();
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

    void PlayerItemComponent::ClearItemLog()
    {
        _itemLogs.clear();
    }

    bool PlayerItemComponent::IsValidVendorPage(int8_t page) const
    {
        return page >= 0 && page < std::ssize(_vendorSaleItems);
    }

    bool PlayerItemComponent::IsEmpty(int8_t page, const ItemSlotRange& range) const
    {
        const ItemSlotStorage* slotStorage = GetInventorySlotStorage(page);
        if (!slotStorage)
        {
            return false;
        }

        return slotStorage->HasEmptySlot(range);
    }

    bool PlayerItemComponent::IsEquipped(EquipmentPosition position) const
    {
        return GetEquipmentItem(position) != nullptr;
    }

    bool PlayerItemComponent::HasInventoryItem(int32_t itemId, int32_t quantity) const
    {
        const auto [begin, end] = _inventoryItemIdIndex.equal_range(itemId);
        if (begin == end)
        {
            return false;
        }

        int32_t sum = 0;

        for (const GameItem& item : std::ranges::subrange(begin, end) | std::views::values | notnull::reference)
        {
            sum += item.GetQuantity();

            if (sum >= quantity)
            {
                return true;
            }
        }

        return false;
    }

    bool PlayerItemComponent::HasVendorSaleItem(int8_t page) const
    {
        assert(IsValidVendorPage(page));

        return _vendorSaleItems[page] != nullptr;
    }

    auto PlayerItemComponent::GetVendorSaleItemSize() const -> int64_t
    {
        return std::ranges::count_if(_vendorSaleItems, [](const GameItem* item) -> bool
            {
                return item != nullptr;
            });
    }

    auto PlayerItemComponent::GetMixItemSize() const -> int64_t
    {
        return std::ssize(_mixItems);
    }

    void PlayerItemComponent::AddOrSubGold(int32_t value)
    {
        _gold += value;

        AddGoldChangeLog(_gold);
    }

    void PlayerItemComponent::SetGold(int32_t value)
    {
        _gold = value;

        AddGoldChangeLog(_gold);
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

        Insert(std::move(item));

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

        Insert(std::move(item));

        return true;
    }

    bool PlayerItemComponent::AddVendorItem(SharedPtrNotNull<GameItem> item, int32_t slot)
    {
        assert(item->HasComponent<ItemPositionComponent>());

        if (slot < 0 || slot >= std::ssize(_vendorSaleItems))
        {
            return false;
        }

        if (_vendorSaleItems[slot])
        {
            return false;
        }

        if (_items.contains(item->GetId()))
        {
            return false;
        }

        ItemPositionComponent& positionComponent = item->GetComponent<ItemPositionComponent>();
        positionComponent.SetPositionType(ItemPositionType::Vendor);
        positionComponent.SetPage(static_cast<int8_t>(slot));

        Insert(std::move(item));

        return true;
    }

    bool PlayerItemComponent::RemoveInventoryItemAll(int32_t itemId, std::vector<game_entity_id_type>* result)
    {
        const auto [begin, end] = _inventoryItemIdIndex.equal_range(itemId);
        if (begin == end)
        {
            return false;
        }

        const auto vector = std::ranges::to<std::vector>(std::ranges::subrange(begin, end) | std::views::values);

        for (GameItem& item : vector | notnull::reference)
        {
            if (result)
            {
                result->emplace_back(item.GetId());
            }

            Erase(item.GetId());
        }

        return true;
    }

    bool PlayerItemComponent::TryRemoveInventoryItem(int32_t itemId, int32_t quantity, std::vector<item_remove_result_type>* result)
    {
        if (quantity <= 0)
        {
            assert(false);

            return false;
        }

        const auto filter = [itemId](const GameItem& item) -> bool
            {
                if (item.GetData().GetId() != itemId)
                {
                    return false;
                }

                return item.GetComponent<ItemPositionComponent>().GetPositionType() == ItemPositionType::Inventory;
            };

        std::vector<game_entity_id_type> removes;
        int32_t sum = 0;

        for (const GameItem& item : _items | std::views::values | notnull::reference | std::views::filter(filter))
        {
            sum += item.GetQuantity();
            removes.push_back(item.GetId());

            if (sum >= quantity)
            {
                break;
            }
        }

        if (sum < quantity)
        {
            return false;
        }

        for (game_entity_id_type removeItemId : removes)
        {
            const auto iter = _items.find(removeItemId);
            assert(iter != _items.end());

            GameItem& targetItem = *iter->second;

            const int32_t targetItemQuantity = targetItem.GetQuantity();

            if (sum >= targetItemQuantity)
            {
                sum -= targetItemQuantity;

                if (result)
                {
                    result->emplace_back(ItemRemoveResultRemove{
                        .itemId = targetItem.GetId(),
                        .itemType = targetItem.GetType(),
                        });
                }

                Erase(removeItemId);
            }
            else if (sum < targetItemQuantity)
            {
                if (result)
                {
                    result->emplace_back(ItemRemoveResultDecrease{
                        .itemId = targetItem.GetId(),
                        .itemType = targetItem.GetType(),
                        .decreaseQuantity = sum,
                    });
                }

                targetItem.SetQuantity(targetItem.GetQuantity() - sum);

                AddItemUpdateQuantityLog(targetItem);

                break;
            }
        }

        return true;
    }

    bool PlayerItemComponent::TryStackInventoryItem(int32_t itemId, int32_t quantity, int32_t& usedQuantity,
        std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>>* result)
    {
        if (quantity <= 0)
        {
            assert(false);

            return false;
        }

        const auto [begin, end] = _inventoryItemIdIndex.equal_range(itemId);
        if (begin == end)
        {
            return false;
        }

        _sortedStackableItemCache.clear();
        _sortedStackableItemCache.insert_range(_sortedStackableItemCache.end(), std::ranges::subrange(begin, end) | std::views::values);
        std::ranges::sort(_sortedStackableItemCache, [](PtrNotNull<const GameItem> lhs, PtrNotNull<const GameItem> rhs) -> bool
            {
                const int32_t lQuantity = lhs->GetQuantity();
                const int32_t rQuantity = rhs->GetQuantity();

                if (lQuantity == rQuantity)
                {
                    const ItemPositionComponent& lPositionComponent = lhs->GetComponent<ItemPositionComponent>();
                    const ItemPositionComponent& rPositionComponent = rhs->GetComponent<ItemPositionComponent>();

                    const int8_t lPage = lPositionComponent.GetPage();
                    const int8_t rPage = rPositionComponent.GetPage();

                    if (lPage == rPage)
                    {
                        const int8_t lY = lPositionComponent.GetY();
                        const int8_t rY = rPositionComponent.GetY();

                        if (lY == rY)
                        {
                            assert(lPositionComponent.GetX() != rPositionComponent.GetX());

                            return lPositionComponent.GetX() < rPositionComponent.GetX();
                        }

                        return lY < rY;
                    }

                    return lPage < rPage;
                }

                return lQuantity < rQuantity;
            });

        int32_t remainQuantity = quantity;

        for (GameItem& item : _sortedStackableItemCache | notnull::reference)
        {
            assert(item.GetData().GetId() == itemId);
            assert(item.GetData().GetMaxOverlapCount() > 1);
            assert(item.GetComponent<ItemPositionComponent>().GetPositionType() == ItemPositionType::Inventory);

            const int32_t maxOverlapCount = item.GetData().GetMaxOverlapCount();
            if (item.GetQuantity() >= maxOverlapCount)
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

        GameItem* equipItem = Mutable(position);
        if (!equipItem)
        {
            return false;
        }

        UpdateItemPosition(*equipItem, ItemPositionUpdatePicked{});

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

        GameItem* pickItem = _pickItem;

        UpdateItemPosition(*pickItem, ItemPositionUpdateEquipment{ .position = position });

        return true;
    }

    bool PlayerItemComponent::SwapPickedItemTo(EquipmentPosition position)
    {
        if (!_pickItem)
        {
            return false;
        }

        GameItem* equipItem = Mutable(position);
        if (!equipItem)
        {
            return false;
        }

        GameItem* pickItem = _pickItem;

        SwapItemPosition(*pickItem, *equipItem);

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

        UpdateItemPosition(*item, ItemPositionUpdatePicked{});

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

        UpdateItemPosition(*_pickItem, ItemPositionUpdateInventory{
            .page = page,
            .x = x,
            .y = y,
            });

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

        GameItem* pickItem = _pickItem;
        GameItem* inventoryItem = *_itemStorageQueryResult.begin();

        RemovePosition(*pickItem);
        RemovePosition(*inventoryItem);

        ItemPositionComponent& inventoryItemPositionComponent = inventoryItem->GetComponent<ItemPositionComponent>();
        inventoryItemPositionComponent.SetPositionType(ItemPositionType::Pick);
        inventoryItemPositionComponent.ResetPosition();

        ItemPositionComponent& pickItemPositionComponent = pickItem->GetComponent<ItemPositionComponent>();
        pickItemPositionComponent.SetPositionType(ItemPositionType::Inventory);
        pickItemPositionComponent.SetPosition(page, x, y);

        AddPosition(*inventoryItem);
        AddPosition(*pickItem);

        AddItemUpdatePositionLog(*pickItem);
        AddItemUpdatePositionLog(*inventoryItem);

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

        UpdateItemPosition(*_pickItem, ItemPositionUpdateQuickSlot{
            .page = page,
            .x = static_cast<int8_t>(slotRange.x),
            .y = static_cast<int8_t>(slotRange.y),
            });

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

        GameItem* pickItem = _pickItem;
        GameItem* quickSlotItem = *_itemStorageQueryResult.begin();

        SwapItemPosition(*pickItem, *quickSlotItem);

        return true;
    }

    bool PlayerItemComponent::LiftVendorItem(int8_t page)
    {
        if (_pickItem)
        {
            return false;
        }

        if (page < 0 || page >= std::ssize(_vendorSaleItems))
        {
            return false;
        }

        GameItem* targetItem = _vendorSaleItems[page];
        UpdateItemPosition(*targetItem, ItemPositionUpdatePicked{});

        return true;
    }

    bool PlayerItemComponent::LowerPickedItemToVendor(int8_t page)
    {
        if (!_pickItem)
        {
            return false;
        }

        if (page < 0 || page >= std::ssize(_vendorSaleItems) || _vendorSaleItems[page])
        {
            return false;
        }

        GameItem* targetItem = _pickItem;
        UpdateItemPosition(*targetItem, ItemPositionUpdateVendor{ .page = page });

        return true;
    }

    bool PlayerItemComponent::SwapPickedItemToVendor(int8_t page)
    {
        if (!_pickItem)
        {
            return false;
        }

        if (page < 0 || page >= std::ssize(_vendorSaleItems) || !_vendorSaleItems[page])
        {
            return false;
        }

        GameItem* pickItem = _pickItem;
        GameItem* vendorSaleItem = _vendorSaleItems[page];

        SwapItemPosition(*pickItem, *vendorSaleItem);

        return true;
    }

    bool PlayerItemComponent::LowerPickedItemToMix()
    {
        if (!_pickItem)
        {
            return false;
        }

        UpdateItemPosition(*_pickItem, ItemPositionUpdateMix{});

        return true;
    }

    void PlayerItemComponent::MoveMixItemToInventory(std::vector<PtrNotNull<const GameItem>>& result)
    {
        for (auto iter = _mixItems.begin(); iter != _mixItems.end(); )
        {
            GameItem* item = iter->second;
            const ItemData& itemData = item->GetData();

            const auto pos = FindEmptyInventoryPosition(itemData.GetWidth(), itemData.GetHeight());
            if (pos.has_value())
            {
                ItemSlotStorage* storage = GetInventorySlotStorage(pos->page);
                assert(storage);

                const ItemSlotRange range{
                    .x = pos->x,
                    .y = pos->y,
                    .xSize = itemData.GetWidth(),
                    .ySize = itemData.GetHeight(),
                };
                assert(storage->HasEmptySlot(range));

                ItemPositionComponent& positionComponent = item->GetComponent<ItemPositionComponent>();
                positionComponent.SetPositionType(ItemPositionType::Inventory);
                positionComponent.SetPosition(pos->page, pos->x, pos->y);

                storage->Set(item, range);
                SUNLIGHT_GAME_DEBUG_REPORT(debug_type, storage->GetDebugString());

                _inventoryItemIdIndex.emplace(item->GetData().GetId(), item);

                AddItemUpdatePositionLog(*item);

                result.push_back(item);

                iter = _mixItems.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    bool PlayerItemComponent::SwapWeaponItem()
    {
        GameItem* mainWeapon = Mutable(EquipmentPosition::Weapon1);
        GameItem* subWeapon = Mutable(EquipmentPosition::Weapon2);

        if (!mainWeapon && !subWeapon)
        {
            return false;
        }

        if (mainWeapon && subWeapon)
        {
            SwapItemPosition(*mainWeapon, *subWeapon);
        }
        else if (mainWeapon)
        {
            UpdateItemPosition(*mainWeapon, ItemPositionUpdateEquipment{ .position = EquipmentPosition::Weapon2 });
        }
        else if (subWeapon)
        {
            UpdateItemPosition(*subWeapon, ItemPositionUpdateEquipment{ .position = EquipmentPosition::Weapon1 });
        }

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

        ItemPositionComponent& positionComponent = item->GetComponent<ItemPositionComponent>();
        positionComponent.SetPositionType(ItemPositionType::Pick);
        positionComponent.ResetPosition();

        Insert(std::move(item));

        return true;
    }

    bool PlayerItemComponent::RemovePickedItem()
    {
        if (!_pickItem)
        {
            return false;
        }

        RemoveItem(_pickItem->GetId());

        return true;
    }

    bool PlayerItemComponent::RemoveItem(game_entity_id_type id)
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return false;
        }

        Erase(id);

        return true;
    }

    auto PlayerItemComponent::ReleaseItem(game_entity_id_type id) -> std::shared_ptr<GameItem>
    {
        if (!_items.contains(id))
        {
            return {};
        }

        return Erase(id);
    }

    auto PlayerItemComponent::ReleaseItemByUId(int64_t id) -> std::shared_ptr<GameItem>
    {
        const auto iter = _itemsUIdIndex.find(id);
        if (iter == _itemsUIdIndex.end())
        {
            return {};
        }

        return ReleaseItem(iter->second->GetId());
    }

    auto PlayerItemComponent::GetInventoryMasks() const -> std::vector<ItemSlotStorageBase<bool>>
    {
        std::vector<ItemSlotStorageBase<bool>> result;
        result.reserve(_inventoryPage);

        for (int64_t i = 0; i < _inventoryPage; ++i)
        {
            result.emplace_back(_inventorySlot[i]->GetMask());
        }

        return result;
    }

    auto PlayerItemComponent::FindItemShared(game_entity_id_type id) -> std::shared_ptr<GameItem>
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            return nullptr;
        }

        return iter->second;
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

    auto PlayerItemComponent::FindMixItemByItemId(int32_t itemId) const -> const GameItem*
    {
        for (const GameItem* mixItem : _mixItems | std::views::values)
        {
            if (mixItem->GetData().GetId() == itemId)
            {
                return mixItem;
            }
        }

        return nullptr;
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

    auto PlayerItemComponent::GetVendorSaleItem(int32_t page) const -> const GameItem*
    {
        if (page < 0 || page >= std::ssize(_vendorSaleItems))
        {
            return nullptr;
        }

        return _vendorSaleItems[page];
    }

    auto PlayerItemComponent::GetWeaponClass() const -> WeaponClassType
    {
        const GameItem* item = GetEquipmentItem(EquipmentPosition::Weapon1);
        if (!item)
        {
            return WeaponClassType::None;
        }

        return static_cast<WeaponClassType>(item->GetData().GetWeaponData()->weaponClass);
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

    auto PlayerItemComponent::GetInventorySlotStorage(int8_t page) const -> const ItemSlotStorage*
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
        case ItemPositionType::Vendor:
            return db::ItemPosType::Vendor;
        case ItemPositionType::Mix:
            return db::ItemPosType::Mix;
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

    void PlayerItemComponent::AddGoldChangeLog(int32_t newGold)
    {
        _itemLogs.emplace_back(db::GoldChange{
            .cid = _cid,
            .gold = newGold,
            });
    }

    void PlayerItemComponent::AddPosition(GameItem& item)
    {
        ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

        const ItemData& data = item.GetData();
        const int8_t page = positionComponent.GetPage();
        const int8_t x = positionComponent.GetX();
        const int8_t y = positionComponent.GetY();

        switch (positionComponent.GetPositionType())
        {
        case ItemPositionType::Inventory:
        {
            ItemSlotStorage* slotStorage = GetInventorySlotStorage(page);
            assert(slotStorage);

            const ItemSlotRange range{
                .x = x,
                .y = y,
                .xSize = data.GetWidth(),
                .ySize = data.GetHeight()
            };

            assert(slotStorage->HasEmptySlot(range));

            slotStorage->Set(&item, range);
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, slotStorage->GetDebugString());

            _inventoryItemIdIndex.emplace(item.GetData().GetId(), &item);
        }
        break;
        case ItemPositionType::Equipment:
        {
            assert(page > static_cast<int8_t>(EquipmentPosition::None) && page < static_cast<int8_t>(EquipmentPosition::Count));

            GameItem*& equipItem = Mutable(static_cast<EquipmentPosition>(page));
            assert(!equipItem);

            equipItem = &item;
        }
        break;
        case ItemPositionType::Pick:
        {
            assert(!_pickItem);

            _pickItem = &item;
        }
        break;
        case ItemPositionType::QuickSlot:
        {
            ItemSlotStorage* slotStorage = GetQuickSlotStorage(page);
            assert(slotStorage);

            const ItemSlotRange range{
                .x = x,
                .y = y,
                .xSize = 1,
                .ySize = 1
            };

            assert(slotStorage->HasEmptySlot(range));

            slotStorage->Set(&item, range);
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, slotStorage->GetDebugString());
        }
        break;
        case ItemPositionType::Vendor:
        {
            assert(page >= 0 && page < std::ssize(_vendorSaleItems));
            assert(!_vendorSaleItems[page]);

            _vendorSaleItems[page] = &item;
        }
        break;
        case ItemPositionType::Mix:
        {
            [[maybe_unused]]
            const bool inserted = _mixItems.try_emplace(item.GetId(), &item).second;
            assert(inserted);
        }
        break;
        case ItemPositionType::Count:
        default:
            assert(false);
        }
    }

    void PlayerItemComponent::RemovePosition(GameItem& item)
    {
        ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

        switch (positionComponent.GetPositionType())
        {
        case ItemPositionType::Inventory:
        {
            GetInventorySlotStorage(positionComponent.GetPage())->Set(nullptr,
                ItemSlotRange{
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
                .xSize = item.GetData().GetWidth(),
                .ySize = item.GetData().GetHeight(),
                });

            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, GetInventorySlotStorage(positionComponent.GetPage())->GetDebugString());

            const auto [begin, end] = _inventoryItemIdIndex.equal_range(item.GetData().GetId());
            assert(begin != end);

            [[maybe_unused]]
            bool erased = false;

            for (auto iter2 = begin; iter2 != end; ++iter2)
            {
                if (PtrNotNull<GameEntity> inventoryItem = iter2->second;
                    inventoryItem == &item)
                {
                    erased = true;

                    _inventoryItemIdIndex.erase(iter2);

                    break;
                }
            }

            assert(erased);
        }
        break;
        case ItemPositionType::Equipment:
        {
            GameItem*& equipItem = Mutable(static_cast<EquipmentPosition>(positionComponent.GetPage()));
            assert(equipItem && equipItem == &item);

            equipItem = nullptr;
        }
        break;
        case ItemPositionType::Pick:
        {
            assert(_pickItem == &item);

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
        case ItemPositionType::Vendor:
        {
            assert(_vendorSaleItems[positionComponent.GetPage()] == &item);

            _vendorSaleItems[positionComponent.GetPage()] = nullptr;
        }
        break;
        case ItemPositionType::Mix:
        {
            [[maybe_unused]]
            const bool erased = _mixItems.erase(item.GetId());
            assert(erased);
        }
        break;
        case ItemPositionType::Count:
        default:
            assert(false);
        }
    }

    void PlayerItemComponent::SwapItemPosition(GameItem& first, GameItem& second)
    {
        RemovePosition(first);
        RemovePosition(second);

        first.GetComponent<ItemPositionComponent>().SwapPosition(second.GetComponent<ItemPositionComponent>());

        AddPosition(first);
        AddPosition(second);

        AddItemUpdatePositionLog(first);
        AddItemUpdatePositionLog(second);
    }

    void PlayerItemComponent::UpdateItemPosition(GameItem& item, item_position_update_type updateType)
    {
        std::visit([this, &item]<typename T>(const T& update)
            {
                RemovePosition(item);

                ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

                if constexpr (std::is_same_v<T, ItemPositionUpdatePicked>)
                {
                    positionComponent.SetPositionType(ItemPositionType::Pick);
                    positionComponent.ResetPosition();
                }
                else if constexpr (std::is_same_v<T, ItemPositionUpdateInventory>)
                {
                    positionComponent.SetPositionType(ItemPositionType::Inventory);
                    positionComponent.SetPosition(update.page, update.x, update.y);
                }
                else if constexpr (std::is_same_v<T, ItemPositionUpdateQuickSlot>)
                {
                    positionComponent.SetPositionType(ItemPositionType::QuickSlot);
                    positionComponent.SetPosition(update.page, update.x, update.y);
                }
                else if constexpr (std::is_same_v<T, ItemPositionUpdateEquipment>)
                {
                    positionComponent.SetPositionType(ItemPositionType::Equipment);
                    positionComponent.ResetPosition();
                    positionComponent.SetPage(static_cast<int8_t>(update.position));
                }
                else if constexpr (std::is_same_v<T, ItemPositionUpdateVendor>)
                {
                    positionComponent.SetPositionType(ItemPositionType::Vendor);
                    positionComponent.ResetPosition();
                    positionComponent.SetPage(update.page);
                }
                else if constexpr (std::is_same_v<T, ItemPositionUpdateMix>)
                {
                    positionComponent.SetPositionType(ItemPositionType::Mix);
                    positionComponent.ResetPosition();
                }
                else
                {
                    static_assert(!sizeof(T), "not implemented");
                }

                AddPosition(item);

            }, updateType);

        AddItemUpdatePositionLog(item);
    }

    void PlayerItemComponent::Insert(SharedPtrNotNull<GameItem> item)
    {
        AddPosition(*item);

        AddItemAddLog(*item);

        const game_entity_id_type id = item->GetId();

        [[maybe_unused]]
        bool inserted = false;

        inserted = _itemsUIdIndex.try_emplace(item->GetUId().value(), item.get()).second;
        assert(inserted);

        inserted = _items.try_emplace(id, std::move(item)).second;
        assert(inserted);
    }

    auto PlayerItemComponent::Erase(game_entity_id_type id) -> std::shared_ptr<GameItem>
    {
        const auto iter = _items.find(id);
        if (iter == _items.end())
        {
            assert(false);

            return {};
        }

        RemovePosition(*iter->second);

        AddItemRemoveLog(*iter->second);

        _itemsUIdIndex.erase(iter->second->GetUId().value());

        std::shared_ptr<GameItem> result = std::move(iter->second);

        _items.erase(iter);

        return result;
    }
}

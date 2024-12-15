#include "player_account_storage_component.h"

#include "sl/generator/game/component/item_position_component.h"
#include "sl/generator/game/contents/item/item_slot_stroage.h"
#include "sl/generator/game/debug/game_debugger.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/service/database/dto/account_storage.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    PlayerAccountStorageComponent::PlayerAccountStorageComponent()
    {
        for (int64_t i = 0; i < std::ssize(_itemStorages); ++i)
        {
            _itemStorages[i] = std::make_unique<ItemSlotStorage>(GameConstant::INVENTORY_WIDTH, GameConstant::INVENTORY_HEIGHT);
        }
    }

    PlayerAccountStorageComponent::~PlayerAccountStorageComponent()
    {
    }

    void PlayerAccountStorageComponent::Initialize(GameEntityIdPublisher& idPublisher,
        const ItemDataProvider& itemDataProvider, const db::dto::AccountStorage& dto)
    {
        assert(_loadPending);

        _loadPending = false;
        _aid = dto.aid;
        _page = dto.page;
        _gold = dto.gold;

        for (const db::dto::AccountStorage::Item& dtoItem : dto.items)
        {
            const ItemData* itemData = itemDataProvider.Find(dtoItem.dataId);
            if (!itemData)
            {
                assert(false);

                continue;
            }

            auto item = std::make_shared<GameItem>(idPublisher, *itemData, dtoItem.quantity);
            item->SetUId(dtoItem.id);
            item->AddComponent(std::make_unique<ItemPositionComponent>());

            ItemPositionComponent& positionComponent = item->GetComponent<ItemPositionComponent>();
            positionComponent.SetPositionType(ItemPositionType::Inventory);
            positionComponent.SetPosition(dtoItem.page, dtoItem.x, dtoItem.y);

            ItemSlotStorage& slotStorage = GetItemSlotStorage(dtoItem.page);

            const ItemSlotRange slotRange{
                .x = dtoItem.x,
                .y = dtoItem.y,
                .xSize = itemData->GetWidth(),
                .ySize = itemData->GetHeight(),
            };

            if (!slotStorage.Contains(slotRange) || !slotStorage.HasEmptySlot(slotRange))
            {
                assert(false);

                continue;
            }

            slotStorage.Set(item.get(), slotRange);

            const game_entity_id_type id = item->GetId();

            _items[id] = std::move(item);
        }
    }

    bool PlayerAccountStorageComponent::IsLoadPending() const
    {
        return _loadPending;
    }

    bool PlayerAccountStorageComponent::HasItemLog() const
    {
        return !_itemLogs.empty();
    }

    void PlayerAccountStorageComponent::FlushItemLogTo(std::vector<db::ItemLog>& dest)
    {
        if (_itemLogs.empty())
        {
            return;
        }

        dest.insert_range(dest.end(), _itemLogs);
        _itemLogs.clear();
    }

    bool PlayerAccountStorageComponent::LowerItem(SharedPtrNotNull<GameItem> item, int8_t page, int8_t x, int8_t y, std::shared_ptr<GameItem>& outLifted)
    {
        if (page < 0 || page >= std::ssize(_itemStorages))
        {
            return false;
        }

        ItemSlotStorage& itemSlotStorage = GetItemSlotStorage(page);

        const ItemSlotRange range{
            .x = x,
            .y = y,
            .xSize = item->GetData().GetWidth(),
            .ySize = item->GetData().GetHeight(),
        };

        if (!itemSlotStorage.Contains(range))
        {
            return false;
        }

        _itemStorageQueryResult.clear();
        itemSlotStorage.Get(_itemStorageQueryResult, range);

        if (!_itemStorageQueryResult.empty())
        {
            if (std::ssize(_itemStorageQueryResult) > 1)
            {
                return false;
            }

            GameItem* lifted = *_itemStorageQueryResult.begin();
            const ItemPositionComponent& liftedItemPositionComponent = lifted->GetComponent<ItemPositionComponent>();

            itemSlotStorage.Set(nullptr, ItemSlotRange{
                .x = liftedItemPositionComponent.GetX(),
                .y = liftedItemPositionComponent.GetY(),
                .xSize = lifted->GetData().GetWidth(),
                .ySize = lifted->GetData().GetHeight(),
                });
            SUNLIGHT_GAME_DEBUG_REPORT(debug_type, itemSlotStorage.GetDebugString());

            AddItemRemoveLog(*lifted);

            const auto iter = _items.find(lifted->GetId());
            assert(iter != _items.end());

            outLifted = std::move(iter->second);
            _items.erase(iter);
        }

        ItemPositionComponent& itemPositionComponent = item->GetComponent<ItemPositionComponent>();
        itemPositionComponent.SetPositionType(ItemPositionType::Inventory);
        itemPositionComponent.SetPosition(page, x, y);

        itemSlotStorage.Set(item.get(), range);
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, itemSlotStorage.GetDebugString());

        AddItemAddLog(*item);

        assert(!_items.contains(item->GetId()));
        _items[item->GetId()] = std::move(item);

        return true;
    }

    auto PlayerAccountStorageComponent::LiftItem(int8_t page, int8_t x, int8_t y) -> std::shared_ptr<GameItem>
    {
        if (page < 0 || page >= std::ssize(_itemStorages))
        {
            return {};
        }

        ItemSlotStorage& itemSlotStorage = GetItemSlotStorage(page);
        GameItem* lifted = itemSlotStorage.Get(x, y);

        if (!lifted)
        {
            return {};
        }

        const ItemPositionComponent& liftedItemPositionComponent = lifted->GetComponent<ItemPositionComponent>();

        itemSlotStorage.Set(nullptr, ItemSlotRange{
                .x = liftedItemPositionComponent.GetX(),
                .y = liftedItemPositionComponent.GetY(),
                .xSize = lifted->GetData().GetWidth(),
                .ySize = lifted->GetData().GetHeight(),
            });
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, itemSlotStorage.GetDebugString());

        AddItemRemoveLog(*lifted);

        const auto iter = _items.find(lifted->GetId());
        assert(iter != _items.end());

        SharedPtrNotNull<GameItem> result = std::move(iter->second);
        _items.erase(iter);

        return result;
    }

    auto PlayerAccountStorageComponent::GetPage() const -> int8_t
    {
        return _page;
    }

    auto PlayerAccountStorageComponent::GetGold() const -> int32_t
    {
        return _gold;
    }

    auto PlayerAccountStorageComponent::GetItemSlotStorage(int8_t page) -> ItemSlotStorage&
    {
        assert(page >= 0 && page < std::ssize(_itemStorages));

        return *_itemStorages[page];
    }

    void PlayerAccountStorageComponent::AddItemAddLog(const GameItem& item)
    {
        const ItemPositionComponent& itemPositionComponent = item.GetComponent<ItemPositionComponent>();

        _itemLogs.emplace_back(db::ItemLogAccountStorageAdd{
            .id = item.GetUId().value(),
            .aid = _aid,
            .itemId = item.GetData().GetId(),
            .quantity = item.GetQuantity(),
            .page = itemPositionComponent.GetPage(),
            .x = itemPositionComponent.GetX(),
            .y = itemPositionComponent.GetY()
            });
    }

    void PlayerAccountStorageComponent::AddItemRemoveLog(const GameItem& item)
    {
        _itemLogs.emplace_back(db::ItemLogAccountStorageRemove{
            .id = item.GetUId().value(),
            });
    }
}

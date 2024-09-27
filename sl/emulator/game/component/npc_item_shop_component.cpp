#include "npc_item_shop_component.h"

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/debug/game_debugger.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/service/gamedata/item/item_data.h"

namespace sunlight
{
    NPCItemShopComponent::NPCItemShopComponent(const ItemShopData& itemShopData)
        : _itemShopData(itemShopData)
    {
        for (int64_t i = 0; i < std::ssize(_itemStorages); ++i)
        {
            _itemStorages[i] = std::make_unique<ItemSlotStorage>(GameConstant::INVENTORY_WIDTH, GameConstant::INVENTORY_HEIGHT);
        }
    }

    void NPCItemShopComponent::Initialize(std::vector<NPCShopItemBucket> itemBuckets)
    {
        _itemBuckets = std::move(itemBuckets);
    }

    bool NPCItemShopComponent::ContainsSyncPlayer(GamePlayer& player) const
    {
        return _syncPlayers.contains(&player);
    }

    void NPCItemShopComponent::AddSyncPlayer(GamePlayer& player)
    {
        assert(!_syncPlayers.contains(&player));

        _syncPlayers.insert(&player);
    }

    void NPCItemShopComponent::RemoveSyncPlayer(GamePlayer& player)
    {
        assert(_syncPlayers.contains(&player));

        _syncPlayers.erase(&player);
    }

    void NPCItemShopComponent::Synchronize(const Buffer& buffer)
    {
        for (GamePlayer& player : _syncPlayers | notnull::reference)
        {
            player.Send(buffer.DeepCopy());
        }
    }

    bool NPCItemShopComponent::Contains(int32_t page, int32_t x, int32_t y) const
    {
        if (page < 0 || page >= std::ssize(_itemStorages))
        {
            return false;
        }

        if (x < 0 || x >= GameConstant::INVENTORY_WIDTH)
        {
            return false;
        }

        if (y < 0 || y >= GameConstant::INVENTORY_HEIGHT)
        {
            return false;
        }

        return true;
    }

    void NPCItemShopComponent::AddItem(SharedPtrNotNull<GameItem> item, const InventoryPosition& position)
    {
        if (!item->HasComponent<ItemPositionComponent>())
        {
            item->AddComponent(std::make_unique<ItemPositionComponent>());
        }

        ItemPositionComponent& positionComponent = item->GetComponent<ItemPositionComponent>();
        positionComponent.SetPositionType(ItemPositionType::Inventory);
        positionComponent.SetPosition(position.page, position.x, position.y);

        ItemSlotStorage& itemSlotStorage = GetItemSlotStorage(position.page);
        itemSlotStorage.Set(item.get(), ItemSlotRange{
            .x = position.x,
            .y = position.y,
            .xSize = item->GetData().GetWidth(),
            .ySize = item->GetData().GetHeight(),
            });

        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, itemSlotStorage.GetDebugString());

        _items[item->GetId()] = std::move(item);
    }

    void NPCItemShopComponent::RemoveItem(game_entity_id_type itemId)
    {
        const auto iter = _items.find(itemId);
        if (iter == _items.end())
        {
            return;
        }

        const ItemPositionComponent& positionComponent = iter->second->GetComponent<ItemPositionComponent>();

        ItemSlotStorage& itemSlotStorage = GetItemSlotStorage(positionComponent.GetPage());
        itemSlotStorage.Set(nullptr, ItemSlotRange{
            .x = positionComponent.GetX(),
            .y = positionComponent.GetY(),
            .xSize = iter->second->GetData().GetWidth(),
            .ySize = iter->second->GetData().GetHeight(),
            });
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, itemSlotStorage.GetDebugString());

        _items.erase(iter);
    }

    auto NPCItemShopComponent::ReleaseItem(game_entity_id_type itemId) -> SharedPtrNotNull<GameItem>
    {
        const auto iter = _items.find(itemId);
        if (iter == _items.end())
        {
            return {};
        }

        const ItemPositionComponent& positionComponent = iter->second->GetComponent<ItemPositionComponent>();

        ItemSlotStorage& itemSlotStorage = GetItemSlotStorage(positionComponent.GetPage());
        itemSlotStorage.Set(nullptr, ItemSlotRange{
            .x = positionComponent.GetX(),
            .y = positionComponent.GetY(),
            .xSize = iter->second->GetData().GetWidth(),
            .ySize = iter->second->GetData().GetHeight(),
            });
        SUNLIGHT_GAME_DEBUG_REPORT(debug_type, itemSlotStorage.GetDebugString());

        std::shared_ptr<GameItem> result = std::move(iter->second);
        _items.erase(iter);

        return result;
    }

    auto NPCItemShopComponent::FindItem(int8_t page, int32_t x, int32_t y) -> GameItem*
    {
        assert(Contains(page, x, y));

        return GetItemSlotStorage(page).Get(x, y);
    }

    auto NPCItemShopComponent::FindItem(int8_t page, int32_t x, int32_t y) const -> const GameItem*
    {
        assert(Contains(page, x, y));

        return GetItemSlotStorage(page).Get(x, y);
    }

    auto NPCItemShopComponent::FindEmptyPosition(int8_t page, int32_t width, int32_t height) const -> std::optional<InventoryPosition>
    {
        assert(page >= 0 && page <= std::ssize(_itemStorages) && width > 0 && height > 0);

        if (const std::optional<std::pair<int32_t, int32_t>> opt = GetItemSlotStorage(page).FindEmpty(width, height);
            opt.has_value())
        {
            return InventoryPosition{
                .page = page,
                .x = static_cast<int8_t>(opt->first),
                .y = static_cast<int8_t>(opt->second),
            };
        }

        return std::nullopt;
    }

    auto NPCItemShopComponent::GetData() const -> const ItemShopData&
    {
        return _itemShopData;
    }

    auto NPCItemShopComponent::GetItemBuckets() const -> const std::vector<NPCShopItemBucket>&
    {
        return _itemBuckets;
    }

    auto NPCItemShopComponent::GetItemCount() const -> int64_t
    {
        return std::ssize(_items);
    }

    auto NPCItemShopComponent::GetItemStorageCount() const -> int8_t
    {
        return static_cast<int8_t>(std::ssize(_itemStorages));
    }

    auto NPCItemShopComponent::GetItemStorageLoadFactor(int8_t page) const -> double
    {
        return GetItemSlotStorage(page).GetLoadFactor();
    }

    auto NPCItemShopComponent::GetItemSlotStorage(int8_t page) -> ItemSlotStorage&
    {
        assert(page >= 0 && page < std::ssize(_itemStorages));

        return *_itemStorages[page];
    }

    auto NPCItemShopComponent::GetItemSlotStorage(int8_t page) const -> const ItemSlotStorage&
    {
        assert(page >= 0 && page < std::ssize(_itemStorages));

        return *_itemStorages[page];
    }
}

#include "player_item_trade_component.h"

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/entity/game_item.h"

namespace sunlight
{
    const std::shared_ptr<GameItem> PlayerItemTradeComponent::null_shared_ptr;

    PlayerItemTradeComponent::PlayerItemTradeComponent()
        : _slotStorage(GameConstant::TRADE_STORAGE_WIDTH, GameConstant::TRADE_STORAGE_HEIGHT)
    {
    }

    void PlayerItemTradeComponent::AddOriginState(int64_t uid, ItemTradeOriginState originState)
    {
        [[maybe_unused]]
        const bool inserted = _originStates.try_emplace(uid, originState).second;
        assert(inserted);
    }

    auto PlayerItemTradeComponent::FindOriginState(int64_t uid) const -> const ItemTradeOriginState*
    {
        const auto iter = _originStates.find(uid);

        return iter != _originStates.end() ? &iter->second : nullptr;
    }

    bool PlayerItemTradeComponent::Contains(game_entity_id_type itemId) const
    {
        return _items.contains(itemId);
    }

    bool PlayerItemTradeComponent::LowerItem(SharedPtrNotNull<GameItem> item, const ItemSlotRange& slotRange, std::shared_ptr<GameItem>& liftedItem)
    {
        if (_items.contains(item->GetId()))
        {
            return false;
        }

        if (!_slotStorage.Contains(slotRange))
        {
            return false;
        }

        _slotStorageQueryResult.clear();
        _slotStorage.Get(_slotStorageQueryResult, slotRange);

        if (!_slotStorageQueryResult.empty())
        {
            if (_slotStorageQueryResult.size() > 2)
            {
                return false;
            }

            PtrNotNull<GameItem> storedItem = *_slotStorageQueryResult.begin();
            const game_entity_id_type removeItemId = storedItem->GetId();

            const auto iter = _items.find(removeItemId);
            assert(iter != _items.end());

            _slotStorage.Set(nullptr, iter->second.second);

            liftedItem = std::move(iter->second.first);
            _items.erase(removeItemId);
        }

        const game_entity_id_type id = item->GetId();

        _slotStorage.Set(item.get(), slotRange);
        _items[id] = std::make_pair(std::move(item), slotRange);

        return true;
    }

    auto PlayerItemTradeComponent::LiftItem(game_entity_id_type itemId) -> std::shared_ptr<GameItem>
    {
        const auto iter = _items.find(itemId);
        if (iter == _items.end())
        {
            return {};
        }

        SharedPtrNotNull<GameItem> item = std::move(iter->second.first);

        _slotStorage.Set(nullptr, iter->second.second);
        _items.erase(iter);

        return item;
    }

    auto PlayerItemTradeComponent::Release(game_entity_id_type itemId) -> std::shared_ptr<GameItem>
    {
        const auto iter = _items.find(itemId);
        if (iter == _items.end())
        {
            return {};
        }

        SharedPtrNotNull<GameItem> item = std::move(iter->second.first);
        _slotStorage.Set(nullptr, iter->second.second);

        _items.erase(iter);

        return item;
    }

    auto PlayerItemTradeComponent::GetItem(int32_t x, int32_t y) const -> const std::shared_ptr<GameItem>&
    {
        const GameItem* item = _slotStorage.Get(x, y);
        if (!item)
        {
            return null_shared_ptr;
        }

        const auto iter = _items.find(item->GetId());
        assert(iter != _items.end());

        return iter->second.first;
    }

    auto PlayerItemTradeComponent::GetGold() const -> int32_t
    {
        return _gold;
    }

    auto PlayerItemTradeComponent::GetItems() -> std::unordered_map<game_entity_id_type, std::pair<SharedPtrNotNull<GameItem>, ItemSlotRange>>&
    {
        return _items;
    }

    auto PlayerItemTradeComponent::GetItems() const -> const std::unordered_map<game_entity_id_type, std::pair<SharedPtrNotNull<GameItem>, ItemSlotRange>>&
    {
        return _items;
    }

    void PlayerItemTradeComponent::SetGold(int32_t gold)
    {
        _gold = gold;
    }
}

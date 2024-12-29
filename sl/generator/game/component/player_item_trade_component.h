#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/item/item_slot_stroage.h"
#include "sl/generator/game/contents/item/item_trade_origin_state.h"
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class PlayerItemTradeComponent final : public GameComponent
    {
    public:
        PlayerItemTradeComponent();

    public:
        void AddOriginState(int64_t uid, ItemTradeOriginState originState);
        auto FindOriginState(int64_t uid) const -> const ItemTradeOriginState*;

    public:
        bool Contains(game_entity_id_type itemId) const;

        bool LowerItem(SharedPtrNotNull<GameItem> item, const ItemSlotRange& slotRange, std::shared_ptr<GameItem>& liftedItem);
        auto LiftItem(game_entity_id_type itemId) -> std::shared_ptr<GameItem>;

        auto Release(game_entity_id_type itemId) -> std::shared_ptr<GameItem>;

        auto GetItem(int32_t x, int32_t y) const -> const std::shared_ptr<GameItem>&;

        auto GetGold() const -> int32_t;
        auto GetItems() -> std::unordered_map<game_entity_id_type, std::pair<SharedPtrNotNull<GameItem>, ItemSlotRange>>&;
        auto GetItems() const -> const std::unordered_map<game_entity_id_type, std::pair<SharedPtrNotNull<GameItem>, ItemSlotRange>>&;

        void SetGold(int32_t gold);

    private:
        std::unordered_map<int64_t, ItemTradeOriginState> _originStates;

        int32_t _gold = 0;

        std::unordered_map<game_entity_id_type, std::pair<SharedPtrNotNull<GameItem>, ItemSlotRange>> _items;
        ItemSlotStorage _slotStorage;
        boost::unordered::unordered_flat_set<PtrNotNull<GameItem>> _slotStorageQueryResult;

        static const std::shared_ptr<GameItem> null_shared_ptr;
    };
}

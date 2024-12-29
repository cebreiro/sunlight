#pragma once
#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/contents/npc/npc_shop_item_bucket.h"
#include "sl/generator/game/contents/item/inventory_position.h"
#include "sl/generator/game/contents/item/item_slot_stroage.h"
#include "sl/generator/game/debug/game_debug_type.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    struct ItemShopData;

    class GamePlayer;
    class GameItem;
    class ItemSlotStorage;
}

namespace sunlight
{
    class NPCItemShopComponent final : public GameComponent
    {
    public:
        explicit NPCItemShopComponent(const ItemShopData& itemShopData);

        void Initialize(std::vector<NPCShopItemBucket> itemBuckets);

    public:
        bool IsVisitedAfterRoll() const;
        bool HasRollTimer() const;
        bool HasSyncPlayer() const;
        bool ContainsSyncPlayer(GamePlayer& player) const;

        void AddSyncPlayer(GamePlayer& player);
        void RemoveSyncPlayer(GamePlayer& player);

        void Synchronize(const Buffer& buffer);

        auto GetSynchronizePlayerCount() const -> int64_t;
        auto GetNextRollTimePoint() const -> game_time_point_type;

        void SetVisitedAfterRoll(bool value);
        void SetRollTimer(bool value);
        void SetNextRollTimePoint(game_time_point_type timePoint);

    public:
        bool Contains(int32_t page, int32_t x, int32_t y) const;

        void AddItem(SharedPtrNotNull<GameItem> item, const InventoryPosition& position);
        void RemoveItem(game_entity_id_type itemId);
        auto ReleaseItem(game_entity_id_type itemId) -> SharedPtrNotNull<GameItem>;

        auto FindItem(int8_t page, int32_t x, int32_t y) -> GameItem*;
        auto FindItem(int8_t page, int32_t x, int32_t y) const -> const GameItem*;
        auto FindEmptyPosition(int8_t page, int32_t width, int32_t height) const -> std::optional<InventoryPosition>;

        void ClearItems();

        auto GetData() const -> const ItemShopData&;
        auto GetItemBuckets() const -> const std::vector<NPCShopItemBucket>&;

        auto GetItemCount() const -> int64_t;
        auto GetItemStorageCount() const -> int8_t;
        auto GetItemStorageLoadFactor(int8_t page) const -> double;
        auto GetItemSlotDebugString(int8_t page) const -> std::string;

        inline auto GetItemRange() const;

    private:
        auto GetItemSlotStorage(int8_t page) -> ItemSlotStorage&;
        auto GetItemSlotStorage(int8_t page) const -> const ItemSlotStorage&;

    private:
        bool _visitedAfterRoll = false;
        bool _hasRollTimer = false;
        game_time_point_type _nextRollTimePoint = game_time_point_type::max();

        const ItemShopData& _itemShopData;
        std::vector<NPCShopItemBucket> _itemBuckets;

        std::unordered_set<PtrNotNull<GamePlayer>> _syncPlayers;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<GameItem>> _items;
        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_NPC_ITEM_SHOP_PAGE_SIZE> _itemStorages;

        static constexpr GameDebugType debug_type = GameDebugType::NPCItemSlot;
    };

    inline auto NPCItemShopComponent::GetItemRange() const
    {
        return _items | std::views::values | notnull::reference;
    }
}

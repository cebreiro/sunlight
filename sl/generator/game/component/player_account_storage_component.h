#pragma once
#include <boost/unordered/unordered_flat_set.hpp>

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/debug/game_debug_type.h"
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/service/database/transaction/item/item_log.h"

namespace sunlight::db::dto
{
    struct AccountStorage;
}

namespace sunlight
{
    class ItemDataProvider;
    class GameEntityIdPublisher;

    class GameItem;
    class ItemSlotStorage;
}

namespace sunlight
{
    class PlayerAccountStorageComponent : public GameComponent
    {
    public:
        PlayerAccountStorageComponent();
        ~PlayerAccountStorageComponent();

    public:
        void Initialize(GameEntityIdPublisher& idPublisher, const ItemDataProvider& itemDataProvider,
            const db::dto::AccountStorage& dto);

        bool IsLoadPending() const;
        bool HasItemLog() const;

        void FlushItemLogTo(std::vector<db::ItemLog>& dest);

    public:
        bool LowerItem(SharedPtrNotNull<GameItem> item, int8_t page, int8_t x, int8_t y, std::shared_ptr<GameItem>& outLifted);
        auto LiftItem(int8_t page, int8_t x, int8_t y) -> std::shared_ptr<GameItem>;

    public:
        auto GetPage() const -> int8_t;
        auto GetGold() const -> int32_t;
        inline auto GetItemRange() const;

    private:
        auto GetItemSlotStorage(int8_t page) -> ItemSlotStorage&;

        void AddItemAddLog(const GameItem& item);
        void AddItemRemoveLog(const GameItem& item);

    private:
        bool _loadPending = true;
        int64_t _aid = 0;
        std::vector<db::ItemLog> _itemLogs;

        int8_t _page = 1;
        int32_t _gold = 0;

        std::unordered_map<game_entity_id_type, SharedPtrNotNull<GameItem>> _items;
        std::array<UniquePtrNotNull<ItemSlotStorage>, GameConstant::MAX_ACCOUNT_STORAGE_PAGE_SIZE> _itemStorages;

        boost::unordered_flat_set<PtrNotNull<GameItem>> _itemStorageQueryResult;

        static constexpr GameDebugType debug_type = GameDebugType::PlayerAccountStorageSlot;
    };

    inline auto PlayerAccountStorageComponent::GetItemRange() const
    {
        return _items | std::views::values | notnull::reference;
    }
}

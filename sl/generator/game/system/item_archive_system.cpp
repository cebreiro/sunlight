#include "item_archive_system.h"

#include <boost/scope/scope_exit.hpp>
#include <boost/container/small_vector.hpp>

#include "sl/generator/game/component/item_ownership_component.h"
#include "sl/generator/game/component/item_position_component.h"
#include "sl/generator/game/component/npc_item_shop_component.h"
#include "sl/generator/game/component/player_account_storage_component.h"
#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_group_component.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/data/sox/item_etc.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/message/creator/item_archive_message_creator.h"
#include "sl/generator/game/message/creator/npc_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/player_appearance_system.h"
#include "sl/generator/game/system/player_index_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/game_item_unique_id_publisher.h"
#include "sl/generator/game/zone/service/game_repository_service.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/service/config/config_provide_service.h"
#include "sl/generator/service/database/database_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/item/item_data.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"
#include "sl/generator/service/gamedata/shop/item_shop_data.h"

namespace sunlight
{
    ItemArchiveSystem::ItemArchiveSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
        , _gameConfig(_serviceLocator.Get<ConfigProvideService>().GetGameConfig())
        , _mt(std::random_device{}())
    {
    }

    void ItemArchiveSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<PlayerIndexSystem>());
        Add(stage.Get<PlayerAppearanceSystem>());

        if (PathFindingSystem* pathFindingSystem = stage.Find<PathFindingSystem>();
            pathFindingSystem)
        {
            Add(*pathFindingSystem);
        }
    }

    bool ItemArchiveSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::ITEMARCHIVEMSG,
            std::bind_front(&ItemArchiveSystem::HandleMessage, this)))
        {
            return false;
        }

        if (!stage.AddSubscriber(ZoneMessageType::ACCOUNTSTORAGE_ARCHIVEMSG,
            std::bind_front(&ItemArchiveSystem::HandleMessage, this)))
        {
            return false;
        }

        return true;
    }

    auto ItemArchiveSystem::GetName() const -> std::string_view
    {
        return "item_archive_system";
    }

    auto ItemArchiveSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<ItemArchiveSystem>();
    }

    bool ItemArchiveSystem::PurchaseStreetVendorItem(GamePlayer& host, GamePlayer& guest, game_entity_id_type itemId, int32_t price)
    {
        bool success = false;
        db::ItemTransaction transaction;

        PlayerItemComponent& hostItemComponent = host.GetItemComponent();
        PlayerItemComponent& guestItemComponent = guest.GetItemComponent();

        do
        {
            if (guestItemComponent.GetGold() < price)
            {
                break;
            }

            const GameItem* targetItem = hostItemComponent.FindItem(itemId);
            if (!targetItem || targetItem->GetComponent<ItemPositionComponent>().GetPositionType() != ItemPositionType::Vendor)
            {
                break;
            }

            const ItemData& itemData = targetItem->GetData();
            const std::optional<InventoryPosition> position = guestItemComponent.FindEmptyInventoryPosition(itemData.GetWidth(), itemData.GetHeight());

            if (!position.has_value())
            {
                break;
            }

            std::shared_ptr<GameItem> instance = hostItemComponent.ReleaseItem(itemId);

            [[maybe_unused]]
            const bool added = guestItemComponent.AddInventoryItem(std::move(instance), &position.value());
            assert(added);

            guestItemComponent.AddOrSubGold(-price);

            hostItemComponent.FlushItemLogTo(transaction.logs);
            guestItemComponent.FlushItemLogTo(transaction.logs);

            guest.Defer(ItemArchiveMessageCreator::CreateInventoryItemAdd(guest, *targetItem));
            guest.Defer(ItemArchiveMessageCreator::CreateGoldAddOrSub(guest, -price));

            success = true;
            
        } while (false);

        if (success)
        {
            _serviceLocator.Get<GameRepositoryService>().Save(host, guest, std::move(transaction));

            AddDummyPacketForInventoryRefresh(guest);
            guest.FlushDeferred();
        }

        return success;
    }

    void ItemArchiveSystem::Purchase(GamePlayer& player, GameNPC& npc, game_entity_id_type targetId, int32_t itemId, int32_t page, int32_t x, int32_t y, int32_t quantity)
    {
        PlayerItemComponent& playerItemComponent = player.GetItemComponent();

        NPCItemShopResult result = NPCItemShopResult::JustFail;

        do
        {
            if (IsExternItemTransactionRunning(player))
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] invalid purchase. extern transaction is running. player: {}",
                        GetName(), player.GetCId()));

                result = NPCItemShopResult::JustFail;

                break;
            }

            if (playerItemComponent.GetPickedItem())
            {
                result = NPCItemShopResult::JustFail;

                break;
            }

            NPCItemShopComponent& itemShopComponent = npc.GetComponent<NPCItemShopComponent>();

            if (!itemShopComponent.Contains(page, x, y))
            {
                SUNLIGHT_LOG_WARN(_serviceLocator,
                    fmt::format("[{}] client invalid request invalid buy item position. player: {}, pos: [{}, {}, {}]",
                        GetName(), player.GetCId(), page, x, y));

                result = NPCItemShopResult::JustFail;

                break;
            }

            if (quantity <= 0)
            {
                SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                    fmt::format("[{}] client hack request. player: {}, quantity: {}",
                        GetName(), player.GetCId(), quantity));

                result = NPCItemShopResult::JustFail;

                break;
            }

            GameItem* targetItem = itemShopComponent.FindItem(static_cast<int8_t>(page), x, y);
            if (!targetItem)
            {
                result = NPCItemShopResult::AlreadySold;

                break;
            }

            if (targetItem->GetId() != targetId || targetItem->GetData().GetId() != itemId || targetItem->GetQuantity() < quantity)
            {
                result = NPCItemShopResult::AlreadySold;

                break;
            }

            const int32_t price = static_cast<int32_t>(std::round(
                static_cast<double>(targetItem->GetData().GetPrice())
                / static_cast<double>(targetItem->GetData().GetMaxOverlapCount())
                * static_cast<double>(quantity)
                * static_cast<double>(itemShopComponent.GetData().sellingFactor)));

            if (playerItemComponent.GetGold() < price)
            {
                result = NPCItemShopResult::NotEnoughGold;

                break;
            }

            if (targetItem->GetQuantity() == quantity)
            {
                itemShopComponent.Synchronize(NPCMessageCreator::CreateNPCItemRemove(npc,
                    targetItem->GetId(), targetItem->GetType()));

                SharedPtrNotNull<GameItem> targetItemShared = itemShopComponent.ReleaseItem(targetItem->GetId());
                assert(targetItem == targetItemShared.get());

                player.Send(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *targetItemShared));

                if (!targetItemShared->HasUId())
                {
                    targetItemShared->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());
                }

                [[maybe_unused]]
                const bool added = playerItemComponent.AddNewPickedItem(std::move(targetItemShared));
                assert(added);
            }
            else
            {
                assert(targetItem->GetQuantity() > quantity);

                const int32_t newQuantity = targetItem->GetQuantity() - quantity;

                itemShopComponent.Synchronize(NPCMessageCreator::CreateNPCItemDecrease(npc,
                    *targetItem, newQuantity));

                targetItem->SetQuantity(newQuantity);

                auto item = std::make_shared<GameItem>(_serviceLocator.Get<GameEntityIdPublisher>(), targetItem->GetData(), quantity);
                item->AddComponent(std::make_unique<ItemPositionComponent>());

                player.Send(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *item));

                item->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());
                playerItemComponent.AddNewPickedItem(std::move(item));
            }

            result = NPCItemShopResult::Success;

            player.Send(ItemArchiveMessageCreator::CreateGoldAddOrSub(player, -price));
            playerItemComponent.AddOrSubGold(-price);
            assert(playerItemComponent.GetGold() > 0);

        } while (false);

        if (result == NPCItemShopResult::Success)
        {
            SaveChanges(player);
        }

        player.Defer(NPCMessageCreator::CreateNPCItemArchiveResult(npc, result));
        player.FlushDeferred();
    }

    void ItemArchiveSystem::SellOwnItem(GamePlayer& player, GameNPC& npc, game_entity_id_type targetId)
    {
        NPCItemShopResult result = NPCItemShopResult::JustFail;

        boost::scope::scope_exit exit([&]()
            {
                if (result == NPCItemShopResult::Success)
                {
                    SaveChanges(player);
                }

                player.Defer(NPCMessageCreator::CreateNPCItemArchiveResult(npc, result));
                player.FlushDeferred();
            });

        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] player invalid request. extern transaction is running. player: {}",
                    GetName(), player.GetCId()));

            result = NPCItemShopResult::JustFail;

            return;
        }

        PlayerItemComponent& playerItemComponent = player.GetItemComponent();

        const GameItem* pickedItem = playerItemComponent.GetPickedItem();
        if (!pickedItem)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] player has no picked item. player: {}",
                    GetName(), player.GetCId()));

            result = NPCItemShopResult::JustFail;

            return;
        }

        if (pickedItem->GetId() != targetId)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] player invalid sell item request. player: {}, target: {}, picked: {}",
                    GetName(), player.GetCId(), targetId, pickedItem->GetId()));

            result = NPCItemShopResult::JustFail;

            return;
        }

        if (!pickedItem->GetData().IsAbleToSell())
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] player item sell request. item is not able to sell. player: {}, item: {}",
                    GetName(), player.GetCId(), pickedItem->GetData().GetId()));

            result = NPCItemShopResult::JustFail;

            return;
        }

        result = NPCItemShopResult::Success;

        [[maybe_unused]]
        const std::shared_ptr<GameItem> released = playerItemComponent.ReleaseItem(pickedItem->GetId());
        assert(released.get() == pickedItem);

        player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, pickedItem->GetId(), pickedItem->GetType()));

        const ItemShopData& shopData = npc.GetComponent<NPCItemShopComponent>().GetData();

        const int32_t price = static_cast<int32_t>(std::round(static_cast<double>(pickedItem->GetData().GetPrice())
            / static_cast<double>(pickedItem->GetData().GetMaxOverlapCount())
            * static_cast<double>(pickedItem->GetQuantity())
            * static_cast<double>(shopData.buyingFactor)));

        playerItemComponent.AddOrSubGold(price);
        player.Defer(ItemArchiveMessageCreator::CreateGoldAddOrSub(player, price));
    }

    void ItemArchiveSystem::AddGold(GamePlayer& player, int32_t value)
    {
        if (value == 0)
        {
            return;
        }

        player.GetItemComponent().AddOrSubGold(value);
        player.Send(ItemArchiveMessageCreator::CreateGoldAddOrSub(player, value));

        SaveChanges(player);
    }

    bool ItemArchiveSystem::AddItem(GamePlayer& player, int32_t itemId, int32_t quantity)
    {
        const ItemDataProvider& itemDataProvider = _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider();
        const ItemData* itemData = itemDataProvider.Find(itemId);
        if (!itemData)
        {
            return false;
        }

        const int32_t addQuantity = std::min(quantity, itemData->GetMaxOverlapCount());

        return AddItem(player, CreateNewGameItem(*itemData, addQuantity));
    }

    bool ItemArchiveSystem::AddItem(GamePlayer& player, SharedPtrNotNull<GameItem> item)
    {
        assert(item->GetQuantity() <= item->GetData().GetMaxOverlapCount());

        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to add item. extern transaction is running. player: {}, item_id: {}",
                    GetName(), player.GetCId(), item->GetData().GetId()));

            return false;
        }

        if (!item->HasComponent<ItemPositionComponent>())
        {
            item->AddComponent<ItemPositionComponent>(std::make_unique<ItemPositionComponent>());
        }

        boost::scope::scope_exit exit([&]()
            {
                if (player.HasDeferred())
                {
                    AddDummyPacketForInventoryRefresh(player);

                    player.FlushDeferred();
                }

                SaveChanges(player);
            });

        const ItemData& itemData = item->GetData();
        PlayerItemComponent& itemComponent = player.GetComponent<PlayerItemComponent>();

        const std::optional<InventoryPosition>& pos = itemComponent.FindEmptyInventoryPosition(itemData.GetWidth(), itemData.GetHeight());
        if (!pos.has_value())
        {
            return false;
        }

        if (!item->HasUId())
        {
            item->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());
        }

        if (!itemComponent.AddInventoryItem(item, &pos.value()))
        {
            return false;
        }

        player.Defer(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *item));

        return true;
    }

    bool ItemArchiveSystem::GainItem(GamePlayer& player, int32_t itemId, int32_t quantity, int32_t& addedQuantity)
    {
        const ItemDataProvider& itemDataProvider = _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider();
        const ItemData* itemData = itemDataProvider.Find(itemId);
        if (!itemData)
        {
            return false;
        }

        const int32_t addQuantity = std::min(quantity, itemData->GetMaxOverlapCount());

        return GainItem(player, CreateNewGameItem(*itemData, addQuantity), addedQuantity);
    }

    bool ItemArchiveSystem::GainItem(GamePlayer& player, SharedPtrNotNull<GameItem> item, int32_t& addedQuantity)
    {
        assert(item->GetQuantity() <= item->GetData().GetMaxOverlapCount());

        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to gain item. extern transaction is running. player: {}, item_id: {}",
                    GetName(), player.GetCId(), item->GetData().GetId()));

            return false;
        }

        if (!item->HasComponent<ItemPositionComponent>())
        {
            item->AddComponent<ItemPositionComponent>(std::make_unique<ItemPositionComponent>());
        }

        PlayerItemComponent& itemComponent = player.GetItemComponent();

        int32_t resultAddedCount = 0;

        boost::scope::scope_exit exit([&]()
            {
                addedQuantity = resultAddedCount;

                if (player.HasDeferred())
                {
                    AddDummyPacketForInventoryRefresh(player);

                    player.FlushDeferred();
                }

                SaveChanges(player);
            });

        const ItemData& itemData = item->GetData();

        if (itemData.GetMaxOverlapCount() > 1)
        {
            while (item->GetQuantity() > 0)
            {
                int32_t usedCount = 0;
                std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>> result;

                if (itemComponent.TryStackInventoryItem(itemData.GetId(), item->GetQuantity(), usedCount, &result))
                {
                    assert(usedCount > 0 && usedCount <= item->GetQuantity());

                    item->SetQuantity(item->GetQuantity() - usedCount);

                    resultAddedCount += usedCount;

                    for (const auto& overlappedItem : result | std::views::keys)
                    {
                        player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, overlappedItem->GetId(), overlappedItem->GetType()));
                        player.Defer(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *overlappedItem));
                    }
                }
                else
                {
                    break;
                }
            }
        }

        if (item->GetQuantity() <= 0)
        {
            assert(item->GetQuantity() == 0);

            return true;
        }

        if (!item->HasUId())
        {
            item->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());
        }

        const std::optional<InventoryPosition>& pos = itemComponent.FindEmptyInventoryPosition(itemData.GetWidth(), itemData.GetHeight());
        if (!pos.has_value())
        {
            return false;
        }

        if (!itemComponent.AddInventoryItem(item, &pos.value()))
        {
            return false;
        }

        player.Defer(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *item));

        resultAddedCount += item->GetQuantity();

        return true;
    }

    bool ItemArchiveSystem::RemoveInventoryItem(GamePlayer& player, int32_t itemId, int32_t quantity)
    {
        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to remove item. extern transaction is running. player: {}, item_id: {}",
                    GetName(), player.GetCId(), itemId));

            return false;
        }

        PlayerItemComponent& itemComponent = player.GetItemComponent();

        _itemRemoveResult.clear();

        if (!itemComponent.TryRemoveInventoryItem(itemId, quantity, &_itemRemoveResult))
        {
            return false;
        }

        for (const item_remove_result_type& element : _itemRemoveResult)
        {
            std::visit([&]<typename T>(const T& result)
                {
                    if constexpr (std::is_same_v<T, ItemRemoveResultDecrease>)
                    {
                        player.Defer(ItemArchiveMessageCreator::CreateItemDecrease(player,
                            result.itemId, result.itemType, result.decreaseQuantity));
                    }
                    else if constexpr (std::is_same_v<T, ItemRemoveResultRemove>)
                    {
                        player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player,
                            result.itemId, result.itemType));
                    }
                    else
                    {
                        static_assert(sizeof(T), "not implemented");
                    }

                }, element);
        }

        SaveChanges(player);

        assert(player.HasDeferred());
        player.FlushDeferred();

        return true;
    }

    void ItemArchiveSystem::RemoveInventoryItemAll(GamePlayer& player, int32_t itemId)
    {
        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to remove item. extern transaction is running. player: {}, item_id: {}",
                    GetName(), player.GetCId(), itemId));

            return;
        }

        if (!player.GetItemComponent().RemoveInventoryItemAll(itemId, &_itemRemoveAllResult))
        {
            return;
        }

        SaveChanges(player);

        for (game_entity_id_type removed : _itemRemoveAllResult)
        {
            player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, removed, GameEntityType::Item));
        }

        player.FlushDeferred();
    }

    bool ItemArchiveSystem::Charge(GamePlayer& player, int32_t cost)
    {
        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to charge item. extern transaction is running. player: {}, cost: {}",
                    GetName(), player.GetCId(), cost));

            return false;
        }

        if (cost <= 0)
        {
            assert(false);

            return false;
        }

        PlayerItemComponent& itemComponent = player.GetItemComponent();
        if (itemComponent.GetGold() < cost)
        {
            return false;
        }

        itemComponent.AddOrSubGold(-cost);
        player.Send(ItemArchiveMessageCreator::CreateGoldAddOrSub(player, -cost));

        return true;
    }

    void ItemArchiveSystem::OpenAccountStorage(GamePlayer& player)
    {
        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to open account storage. extern transaction is running. player: {}",
                    GetName(), player.GetCId()));

            return;
        }

        PlayerAccountStorageComponent* accountStorageComponent = player.FindComponent<PlayerAccountStorageComponent>();
        if (!accountStorageComponent)
        {
            auto temp = std::make_unique<PlayerAccountStorageComponent>();
            assert(temp->IsLoadPending());

            accountStorageComponent = temp.get();
            player.AddComponent(std::move(temp));

            _serviceLocator.Get<GameRepositoryService>().LoadAccountStorage(player,
                [this, cid = player.GetCId()](const db::dto::AccountStorage& storage)
                {
                    OnCompleteLoadAccountStorage(cid, storage);
                });

            player.Send(ItemArchiveMessageCreator::CreateAccountStorageOpening(player));

            return;
        }

        if (accountStorageComponent->IsLoadPending())
        {
            return;
        }

        player.Send(ItemArchiveMessageCreator::CreateAccountStorageInit(player));
    }

    void ItemArchiveSystem::OnItemMixSuccess(GamePlayer& player, const ItemData& resultItemData, int32_t resultItemCount, std::span<const ItemMixMaterial> materials)
    {
        bool success = false;

        do
        {
            PlayerItemComponent& itemComponent = player.GetItemComponent();

            for (const ItemMixMaterial& material : materials)
            {
                const GameItem* item = itemComponent.FindItem(material.itemId);
                if (!item)
                {
                    assert(false);

                    continue;
                }

                if (item->GetQuantity() > material.quantity)
                {
                    itemComponent.DecreaseItemQuantity(material.itemId, material.quantity);
                }
                else
                {
                    itemComponent.RemoveItem(material.itemId);
                }
            }

            success = true;

            auto resultItem = CreateNewGameItem(resultItemData, resultItemCount);
            resultItem->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());

            GameItem* resultItemPtr = resultItem.get();

            [[maybe_unused]]
            bool added = false;

            const std::optional<InventoryPosition> pos = itemComponent.FindEmptyInventoryPosition(resultItemData.GetWidth(), resultItemData.GetHeight());
            if (pos.has_value())
            {
                added = itemComponent.AddInventoryItem(std::move(resultItem), &pos.value());

                player.Defer(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *resultItemPtr));

                AddDummyPacketForInventoryRefresh(player);

                player.FlushDeferred();
            }
            else if (itemComponent.GetPickedItem() == nullptr)
            {
                added = itemComponent.AddNewPickedItem(std::move(resultItem));

                player.Send(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *resultItemPtr));
            }
            else
            {
                // TODO: drop item with ownership
            }

            assert(added);

        } while (false);

        if (success)
        {
            SaveChanges(player);
        }
    }

    void ItemArchiveSystem::OnItemMixFail(GamePlayer& player, std::span<const ItemMixMaterial> materials)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        for (const ItemMixMaterial& material : materials)
        {
            const GameItem* item = itemComponent.FindItem(material.itemId);
            if (!item)
            {
                assert(false);

                continue;
            }

            if (item->GetQuantity() > material.quantity)
            {
                itemComponent.DecreaseItemQuantity(material.itemId, material.quantity);
            }
            else
            {
                itemComponent.RemoveItem(material.itemId);
            }
        }

        SaveChanges(player);
    }

    bool ItemArchiveSystem::OnVendorSaleStorageClick(GamePlayer& player, int32_t index)
    {
        if (IsExternItemTransactionRunning(player))
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] fail to lower item to vendor sale storage. extern transaction is running. player: {}",
                    GetName(), player.GetCId()));

            return false;
        }

        bool result = false;
        const int8_t page = static_cast<int8_t>(index);
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        do
        {
            if (!itemComponent.IsValidVendorPage(page))
            {
                break;
            }

            if (const GameItem* pickedItem = itemComponent.GetPickedItem(); pickedItem)
            {
                if (itemComponent.HasVendorSaleItem(page))
                {
                    [[maybe_unused]]
                    const bool swapped = itemComponent.SwapPickedItemToVendor(page);
                    assert(swapped);

                    const GameItem* liftedItem = itemComponent.GetPickedItem();
                    assert(liftedItem);

                    player.Send(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *liftedItem));
                }
                else
                {
                    [[maybe_unused]]
                    const bool lowered = itemComponent.LowerPickedItemToVendor(page);
                    assert(lowered);

                    player.Send(ItemArchiveMessageCreator::CreateItemRemove(player, pickedItem->GetId(), pickedItem->GetType()));
                }
            }
            else
            {
                if (!itemComponent.GetVendorSaleItem(page))
                {
                    return false;
                }

                [[maybe_unused]]
                const bool lifted = itemComponent.LiftVendorItem(page);
                assert(lifted);

                player.Send(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *itemComponent.GetPickedItem()));
            }

            result = true;
            
        } while (false);

        if (result)
        {
            SaveChanges(player);
        }

        return result;
    }

    bool ItemArchiveSystem::IsValid(EquipmentPosition position, sox::EquipmentType soxType)
    {
        switch (position)
        {
        case EquipmentPosition::Hat:
            return soxType == sox::EquipmentType::Hat;
        case EquipmentPosition::Jacket:
            return soxType == sox::EquipmentType::Jacket;
        case EquipmentPosition::Gloves:
            return soxType == sox::EquipmentType::Gloves;
        case EquipmentPosition::Pants:
            return soxType == sox::EquipmentType::Pants;
        case EquipmentPosition::Shoes:
            return soxType == sox::EquipmentType::Shoes;
        case EquipmentPosition::Weapon1:
        case EquipmentPosition::Weapon2:
            return soxType == sox::EquipmentType::Weapon;
        case EquipmentPosition::Necklace:
            return soxType == sox::EquipmentType::Necklace;
        case EquipmentPosition::Ring1:
        case EquipmentPosition::Ring2:
            return soxType == sox::EquipmentType::Ring;
        case EquipmentPosition::Bullet:
            return soxType == sox::EquipmentType::Bullet;
        case EquipmentPosition::None:
        case EquipmentPosition::Dummy:
        case EquipmentPosition::Count:
        default:;
        }

        return false;
    }

    void ItemArchiveSystem::OnWeaponSwap(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        if (!itemComponent.SwapWeaponItem())
        {
            return;
        }

        assert(itemComponent.HasItemLog());
        SaveChanges(player);

        // TODO: update stat

        Get<PlayerAppearanceSystem>().UpdateEquipmentAppearance(player);

        Get<EntityViewRangeSystem>().VisitPlayer(player,
            [&](GamePlayer& target)
            {
                target.Defer(GamePlayerMessageCreator::CreatePlayerWeaponSwap(player, itemComponent.FindEquipmentItem(EquipmentPosition::Weapon1)));
                target.Defer(GamePlayerMessageCreator::CreateRemovePlayerWeaponChange(player));

                target.FlushDeferred();
            });
    }

    void ItemArchiveSystem::TryRollbackMixItem(GamePlayer& player)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();
        if (itemComponent.GetMixItemSize() <= 0)
        {
            return;
        }

        std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>> stackResult;
        boost::container::small_vector<game_entity_id_type, 4> removeItems;

        for (GameItem& mixItem : itemComponent.GetMixItems())
        {
            const ItemData& itemData = mixItem.GetData();
            const int32_t quantity = mixItem.GetQuantity();

            if (itemData.GetMaxOverlapCount() > 1)
            {
                stackResult.clear();
                int32_t usedQuantity = 0;

                if (itemComponent.TryStackInventoryItem(itemData.GetId(), quantity, usedQuantity, &stackResult))
                {
                    assert(usedQuantity > 0 && usedQuantity <= quantity);

                    if (usedQuantity >= quantity)
                    {
                        removeItems.push_back(mixItem.GetId());
                    }
                    else
                    {
                        itemComponent.SetItemQuantity(mixItem.GetId(), quantity - usedQuantity);
                    }

                    for (const auto& overlappedItem : stackResult | std::views::keys)
                    {
                        player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, overlappedItem->GetId(), overlappedItem->GetType()));
                        player.Defer(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *overlappedItem));
                    }
                }
            }
        }

        for (const game_entity_id_type id : removeItems)
        {
            itemComponent.RemoveItem(id);
        }

        std::vector<PtrNotNull<const GameItem>> moveItems;
        itemComponent.MoveMixItemToInventory(moveItems);

        for (const GameItem& item : moveItems | notnull::reference)
        {
            player.Defer(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, item));
        }

        if (player.HasDeferred())
        {
            AddDummyPacketForInventoryRefresh(player);

            player.FlushDeferred();
        }

        SaveChanges(player);
    }

    bool ItemArchiveSystem::OnMixItemLower(GamePlayer& player, game_entity_id_type itemId)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        const GameItem* pickedItem = itemComponent.GetPickedItem();
        if (pickedItem->GetId() != itemId)
        {
            return false;
        }

        boost::scope::scope_exit exit([this, &player]()
            {
                SaveChanges(player);
            });

        const ItemData& itemData = pickedItem->GetData();

        if (const auto* etcData = itemData.GetEtcData();
            etcData && etcData->maxOverlapCount > 1)
        {
            if (const GameItem* mixItem = itemComponent.FindMixItemByItemId(itemData.GetId());
                mixItem)
            {
                const int32_t mixItemQuantity = mixItem->GetQuantity();
                const int32_t addQuantity = std::min(itemData.GetMaxOverlapCount() - mixItemQuantity, pickedItem->GetQuantity());

                [[maybe_unused]]
                const bool stacked = itemComponent.IncreaseItemQuantity(mixItem->GetId(), addQuantity);
                assert(stacked);

                if (addQuantity >= pickedItem->GetQuantity())
                {
                    player.Send(ItemArchiveMessageCreator::CreateItemRemove(player, pickedItem->GetId(), pickedItem->GetType()));

                    itemComponent.RemovePickedItem();
                }
                else
                {
                    itemComponent.DecreaseItemQuantity(pickedItem->GetId(), addQuantity);
                }

                return true;
            }
        }

        player.Send(ItemArchiveMessageCreator::CreateItemRemove(player, pickedItem->GetId(), pickedItem->GetType()));
        itemComponent.LowerPickedItemToMix();

        return true;
    }

    bool ItemArchiveSystem::OnMixItemLift(GamePlayer& player, game_entity_id_type itemId)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        if (!itemComponent.LiftItem(itemId))
        {
            return false;
        }

        const GameItem* pickedItem = itemComponent.GetPickedItem();
        assert(pickedItem);

        player.Send(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *pickedItem));

        return true;
    }

    void ItemArchiveSystem::OnCompleteLoadAccountStorage(int64_t cid, const db::dto::AccountStorage& dto)
    {
        GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(cid);
        if (!player)
        {
            return;
        }

        PlayerAccountStorageComponent* accountStorageComponent = player->FindComponent<PlayerAccountStorageComponent>();
        if (!accountStorageComponent || !accountStorageComponent->IsLoadPending())
        {
            return;
        }

        accountStorageComponent->Initialize(
            _serviceLocator.Get<GameEntityIdPublisher>(),
            _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider(), dto);

        player->Send(ItemArchiveMessageCreator::CreateAccountStorageInit(*player));
    }

    void ItemArchiveSystem::HandleMessage(const ZoneMessage& message)
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;
        game_entity_id_type targetId = message.targetId;
        GameEntityType targetType = message.targetType;

        bool success = true;
        const auto subType = reader.Read<ZoneMessageType>();

        if (!IsAllowed(player, subType))
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] player request unallowable item message. player: {}, type: {}, buffer: {}",
                    GetName(), player.GetCId(), ToString(subType), reader.GetBuffer().ToString()));

            player.GetClient().Disconnect();

            return;
        }

        boost::scope::scope_exit exit([&]()
            {
                if (success && !IsExternItemTransactionRunning(player))
                {
                    SaveChanges(player);
                }

                switch (message.type)
                {
                case ZoneMessageType::ITEMARCHIVEMSG:
                {
                    player.Defer(ItemArchiveMessageCreator::CreateArchiveResult(player, success, subType));
                }
                break;
                case ZoneMessageType::ACCOUNTSTORAGE_ARCHIVEMSG:
                {
                    player.Defer(ItemArchiveMessageCreator::CreateAccountStorageUnlock(player));
                }
                break;
                }

                player.FlushDeferred();
            });

        switch (subType)
        {
        case ZoneMessageType::ITEMARCHIVE_INIT:
        {
            auto buffer = ItemArchiveMessageCreator::CreateInit(player);

            [[maybe_unused]]
            const auto size = buffer.GetSize();

            player.Defer(std::move(buffer));
        }
        break;
        case ZoneMessageType::ITEMARCHIVE_LIFTITEM:
        {
            [[maybe_unused]]
            const auto [targetItemId, targetItemType] = reader.ReadInt64();
            const int32_t quantity = reader.Read<int32_t>();

            if (targetItemId == 0 || targetItemId == 2)
            {
                // dummy item for preventing quick slot item and skill overlapped

                success = true;

                break;
            }

            success = HandleLiftItem(player, game_entity_id_type(targetItemId), quantity);
        }
        break;
        case ZoneMessageType::ITEMARCHIVE_LOWERITEM:
        {
            const int32_t page = reader.Read<int32_t>();
            const int32_t x = reader.Read<int32_t>();
            const int32_t y = reader.Read<int32_t>();

            const auto [pickedItemId, pickedItemType] = reader.ReadInt64();
            const auto [invenItemId, invenItemType] = reader.ReadInt64();

            success = HandleLowerItem(player, game_entity_id_type(pickedItemId), game_entity_id_type(invenItemId),
                static_cast<int8_t>(page), static_cast<int8_t>(x), static_cast<int8_t>(y));
        }
        break;
        case ZoneMessageType::ITEMARCHIVE_LOWERITEM_QI:
        {
            const int32_t page = reader.Read<int32_t>();
            const int32_t x = reader.Read<int32_t>();
            const int32_t y = reader.Read<int32_t>();

            const auto [pickedItemId, pickedItemType] = reader.ReadInt64();
            const auto [invenItemId, invenItemType] = reader.ReadInt64();

            success = HandleLowerItemToQuickSlot(player, game_entity_id_type(pickedItemId), game_entity_id_type(invenItemId),
                static_cast<int8_t>(page), static_cast<int8_t>(x), static_cast<int8_t>(y));

            break;
        }
        case ZoneMessageType::ITEMARCHIVE_LOWERWEAR:
        {
            const int32_t position = reader.Read<int32_t>();
            if (position < 0 || position >= static_cast<int32_t>(EquipmentPosition::Count))
            {
                
            }

            const auto [equipItemId, equipItemType] = reader.ReadInt64();
            if (equipItemId == 0)
            {
                success = HandleLiftEquipment(player, static_cast<EquipmentPosition>(position));
            }
            else
            {
                success = HandleLowerEquipment(player, game_entity_id_type(equipItemId), static_cast<EquipmentPosition>(position));
            }
        }
        break;
        case ZoneMessageType::SLV2_DESTROY_PICKED_ITEM:
        {
            const auto [pickedItemId, pickedItemType] = reader.ReadInt64();

            success = HandleDestroyPickedItem(player, game_entity_id_type(pickedItemId));
        }
        break;
        case ZoneMessageType::ITEMARCHIVE_DROPITEM:
        {
            const auto [pickedItemId, pickedItemType] = reader.ReadInt64();

            success = HandleDropPickedItem(player, game_entity_id_type(pickedItemId));
        }
        break;
        case ZoneMessageType::ACCOUNTSTORAGE_ARCHIVE_LOWERITEM:
        {
            const int32_t page = reader.Read<int32_t>();
            const int32_t x = reader.Read<int32_t>();
            const int32_t y = reader.Read<int32_t>();

            const auto [pickedItemId, pickedItemType] = reader.ReadInt64();

            success = HandleAccountStorageLowerItem(player,
                static_cast<int8_t>(page),
                static_cast<int8_t>(x),
                static_cast<int8_t>(y),
                game_entity_id_type(pickedItemId));

        }
        break;
        case ZoneMessageType::ACCOUNTSTORAGE_ARCHIVE_TERMINATE:
        {
            // do nothing
        }
        break;
        default:
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] unhandled zone message. player: {}, type: {}, target: [{}, {}], buffer: {}",
                    GetName(), player.GetCId(), ToString(subType), targetId, ToString(targetType), reader.GetBuffer().ToString()));
        }
    }

    bool ItemArchiveSystem::HandleLiftItem(GamePlayer& player, game_entity_id_type targetItemId, int32_t quantity)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        if (itemComponent.GetPickedItem())
        {
            LogHandleItemError(__FUNCTION__, "picked item already exists");

            return false;
        }

        const GameItem* targetItem = itemComponent.FindItem(targetItemId);
        if (!targetItem)
        {
            LogHandleItemError(__FUNCTION__, fmt::format("invalid request. player: {}, item_id: {}, quantity: {}",
                player.GetCId(), targetItemId, quantity));

            return false;
        }

        if (quantity < 0 || targetItem->GetQuantity() == quantity)
        {
            if (!itemComponent.LiftItem(targetItemId))
            {
                LogHandleItemError(__FUNCTION__, fmt::format("fail to lift inventory item. item_id: {}, quantity: {}",
                    targetItemId, quantity));

                return false;
            }

            player.Defer(ItemArchiveMessageCreator::CreateItemLift(player, *itemComponent.GetPickedItem()));
        }
        else
        {
            auto newPickedItem = CreateNewGameItem(targetItem->GetData(), quantity);
            newPickedItem->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());

            [[maybe_unused]]
            bool success = itemComponent.DecreaseItemQuantity(targetItemId, quantity);
            assert(success);

            success = itemComponent.AddNewPickedItem(std::move(newPickedItem));
            assert(success);

            player.Defer(ItemArchiveMessageCreator::CreateItemLift(player, *itemComponent.GetPickedItem(),
                *targetItem, quantity));
        }

        return true;
    }

    bool ItemArchiveSystem::HandleLowerItem(GamePlayer& player, game_entity_id_type lowerItemId,
        game_entity_id_type invenItemId, int8_t page, int8_t x, int8_t y)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        const GameItem* picked = itemComponent.GetPickedItem();
        if (!picked)
        {
            LogHandleItemError(__FUNCTION__, "picked item is null");

            return false;
        }

        if (picked->GetId() != lowerItemId)
        {
            LogHandleItemError(__FUNCTION__, "invalid request. requested item is not picked");

            return false;
        }

        if (invenItemId.Unwrap() == 0)
        {
            if (!itemComponent.LowerPickedItemTo(page, x, y))
            {
                LogHandleItemError(__FUNCTION__, fmt::format("fail to lower item. pos: [{}, {}, {}]",
                    page, x, y));

                return false;
            }
        }
        else
        {
            const GameItem* invenItem = itemComponent.FindInventoryItem(invenItemId);
            if (!invenItem)
            {
                LogHandleItemError(__FUNCTION__, fmt::format("fail to find lift item. id: {}, pos: [{}, {}, {}]",
                    invenItemId, page, x, y));

                return false;
            }

            const int32_t maxOverlapCount = picked->GetData().GetMaxOverlapCount();

            if (picked->GetData().GetId() == invenItem->GetData().GetId() && maxOverlapCount > 1)
            {
                if (invenItem->GetQuantity() >= maxOverlapCount)
                {
                    return true;
                }

                const int32_t sum = picked->GetQuantity() + invenItem->GetQuantity();

                if (sum > maxOverlapCount)
                {
                    [[maybe_unused]]
                    bool result = itemComponent.SetItemQuantity(invenItemId, maxOverlapCount);
                    assert(result);

                    result = itemComponent.SetItemQuantity(picked->GetId(), sum - maxOverlapCount);
                    assert(result);
                }
                else
                {
                    [[maybe_unused]]
                    bool result = itemComponent.SetItemQuantity(invenItemId, sum);
                    assert(result);

                    result = itemComponent.RemoveItem(picked->GetId());
                    assert(result);
                }
            }
            else
            {
                if (!itemComponent.SwapPickedItemTo(page, x, y))
                {
                    LogHandleItemError(__FUNCTION__, fmt::format("fail to swap picked item. pos: [{}, {}, {}]",
                        page, x, y));

                    return false;
                }
            }
        }

        return true;
    }

    bool ItemArchiveSystem::HandleLowerItemToQuickSlot(GamePlayer& player, game_entity_id_type lowerItemId,
        game_entity_id_type quickSlotItemId, int8_t page, int8_t x, int8_t y)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        const GameItem* picked = itemComponent.GetPickedItem();
        if (!picked)
        {
            LogHandleItemError(__FUNCTION__, "picked item is null");

            return false;
        }

        if (picked->GetId() != lowerItemId)
        {
            LogHandleItemError(__FUNCTION__, "invalid request. requested item is not picked");

            return false;
        }

        if (quickSlotItemId.Unwrap() == 0)
        {
            if (!itemComponent.LowerPickedItemToQuickSlot(page, x, y))
            {
                LogHandleItemError(__FUNCTION__, fmt::format("fail to lower item. pos: [{}, {}, {}]",
                    page, x, y));

                return false;
            }
        }
        else
        {
            const GameItem* quickSlotItem = itemComponent.FindQuickSlotItem(quickSlotItemId);
            if (!quickSlotItem)
            {
                LogHandleItemError(__FUNCTION__, fmt::format("fail to find lift item. id: {}, pos: [{}, {}, {}]",
                    quickSlotItemId, page, x, y));

                return false;
            }

            const int32_t maxOverlapCount = picked->GetData().GetMaxOverlapCount();

            if (picked->GetData().GetId() == quickSlotItem->GetData().GetId() && maxOverlapCount > 1)
            {
                if (quickSlotItem->GetQuantity() >= maxOverlapCount)
                {
                    return true;
                }

                const int32_t sum = picked->GetQuantity() + quickSlotItem->GetQuantity();

                if (sum > maxOverlapCount)
                {
                    [[maybe_unused]]
                    bool result = itemComponent.SetItemQuantity(quickSlotItemId, maxOverlapCount);
                    assert(result);

                    result = itemComponent.SetItemQuantity(picked->GetId(), sum - maxOverlapCount);
                    assert(result);
                }
                else
                {
                    [[maybe_unused]]
                    bool result = itemComponent.SetItemQuantity(quickSlotItemId, sum);
                    assert(result);

                    result = itemComponent.RemoveItem(picked->GetId());
                    assert(result);
                }
            }
            else
            {
                if (!itemComponent.SwapPickedItemToQuickSlot(page, x, y))
                {
                    LogHandleItemError(__FUNCTION__, fmt::format("fail to swap picked item. pos: [{}, {}, {}]",
                        page, x, y));

                    return false;
                }
            }
        }

        return true;
    }

    bool ItemArchiveSystem::HandleLiftEquipment(GamePlayer& player, EquipmentPosition position)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        if (!player.GetItemComponent().LiftEquipItem(position))
        {
            return false;
        }

        Get<PlayerAppearanceSystem>().UpdateEquipmentAppearance(player);
        Get<PlayerStatSystem>().OnItemUnequip(player, *itemComponent.GetPickedItem());

        Get<EntityViewRangeSystem>().VisitPlayer(player,
            [&](GamePlayer& other)
            {
                if (other.GetId() == player.GetId())
                {
                    return;
                }

                if (position == EquipmentPosition::Hat)
                {
                    const PlayerAppearanceComponent& appearanceComponent = player.GetAppearanceComponent();

                    other.Defer(GamePlayerMessageCreator::CreatePlayerEquipmentChange(player, position, appearanceComponent.GetHair(), appearanceComponent.GetHairColor()));
                }
                else
                {
                    other.Defer(GamePlayerMessageCreator::CreatePlayerEquipmentChange(player, position, 0, 0));
                }

                if (position == EquipmentPosition::Weapon1)
                {
                    other.Defer(GamePlayerMessageCreator::CreateRemovePlayerWeaponChange(player));
                }

                other.FlushDeferred();
            });

        return true;
    }

    bool ItemArchiveSystem::HandleLowerEquipment(GamePlayer& player, game_entity_id_type equipItemId, EquipmentPosition position)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        const GameItem* picked = itemComponent.GetPickedItem();
        if (!picked)
        {
            LogHandleItemError(__FUNCTION__, "picked item is null");

            return false;
        }

        if (picked->GetId() != equipItemId)
        {
            LogHandleItemError(__FUNCTION__, "invalid request. requested item is not picked");

            return false;
        }

        if (!IsValid(position, picked->GetData().GetEquipmentType()))
        {
            LogHandleItemError(__FUNCTION__, fmt::format("invalid equip_position. request: {}, data: {}",
                ToString(position), ToString(picked->GetData().GetEquipmentType())));

            return false;
        }

        if (itemComponent.IsEquipped(position))
        {
            if (!itemComponent.SwapPickedItemTo(position))
            {
                LogHandleItemError(__FUNCTION__, "fail to lower picked item to pos");

                return false;
            }

            Get<PlayerStatSystem>().OnItemEquipmentChange(player, *itemComponent.GetPickedItem(), *itemComponent.GetEquipmentItem(position));
        }
        else
        {
            if (!itemComponent.LowerPickedItemTo(position))
            {
                LogHandleItemError(__FUNCTION__, "fail to lower picked item to pos");

                return false;
            }

            Get<PlayerStatSystem>().OnItemEquip(player, *itemComponent.GetEquipmentItem(position));
        }

        Get<PlayerAppearanceSystem>().UpdateEquipmentAppearance(player);

        const GameItem* equipItem = itemComponent.GetEquipmentItem(position);
        assert(equipItem);

        Get<EntityViewRangeSystem>().VisitPlayer(player,
            [&, equipItem](GamePlayer& other)
            {
                if (other.GetId() == player.GetId())
                {
                    return;
                }

                other.Send(GamePlayerMessageCreator::CreatePlayerEquipmentChange(player, position,
                    equipItem->GetData().GetModelId(), equipItem->GetData().GetModelColor()));

                if (position == EquipmentPosition::Weapon1)
                {
                    other.Send(GamePlayerMessageCreator::CreateRemovePlayerWeaponChange(player));
                }
            });

        return true;
    }

    bool ItemArchiveSystem::HandleDestroyPickedItem(GamePlayer& player, game_entity_id_type pickedItemId)
    {
        PlayerItemComponent& playerItemComponent = player.GetItemComponent();

        const GameItem* pickedItem = playerItemComponent.GetPickedItem();
        if (!pickedItem || pickedItem->GetId() != pickedItemId)
        {
            LogHandleItemError(__FUNCTION__, fmt::format("invalid request. player: {}, target_item: {}",
                player.GetCId(), pickedItemId));

            return false;
        }

        const GameEntityType type = pickedItem->GetType();

        if (playerItemComponent.RemoveItem(pickedItemId))
        {
            player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, pickedItemId, type));

            return true;
        }

        return false;
    }

    bool ItemArchiveSystem::HandleDropPickedItem(GamePlayer& player, game_entity_id_type pickedItemId)
    {
        PlayerItemComponent& playerItemComponent = player.GetItemComponent();

        const GameItem* targetItem = playerItemComponent.GetPickedItem();
        if (!targetItem || targetItem->GetId() != pickedItemId)
        {
            LogHandleItemError(__FUNCTION__, fmt::format("invalid request. player: {}, target_item: {}",
                player.GetCId(), pickedItemId));

            return false;
        }

        auto pickedItem = playerItemComponent.ReleaseItem(pickedItemId);

        if (!pickedItem->HasComponent<ItemOwnershipComponent>())
        {
            pickedItem->AddComponent(std::make_unique<ItemOwnershipComponent>());
        }

        auto& ownershipComponent = pickedItem->GetComponent<ItemOwnershipComponent>();
        ownershipComponent.Clear();

        ownershipComponent.Add(player.GetCId());
        ownershipComponent.SetEndTimePoint(GameTimeService::Now() + _gameConfig.dropItemOwnershipSeconds);

        const Eigen::Vector2f& playerPosition = player.GetSceneObjectComponent().GetPosition();
        Eigen::Vector2f destPosition;

        if (PathFindingSystem* pathFindingSystem = Find<PathFindingSystem>();
            !pathFindingSystem || pathFindingSystem->GetRandPositionInCircle(destPosition, playerPosition, GameConstant::MONSTER_DROP_ITEM_RADIUS))
        {
            constexpr int32_t range = static_cast<int32_t>(GameConstant::MONSTER_DROP_ITEM_RADIUS);

            std::uniform_int_distribution<int32_t> dist(-range, range);

            destPosition = playerPosition;
            destPosition.x() += static_cast<float>(dist(_mt));
            destPosition.y() += static_cast<float>(dist(_mt));
        }

        Get<SceneObjectSystem>().SpawnItem(std::move(pickedItem), playerPosition, destPosition, _gameConfig.dropItemKeepSeconds);

        return true;
    }

    bool ItemArchiveSystem::HandleAccountStorageLowerItem(GamePlayer& player, int8_t page, int8_t x, int8_t y, game_entity_id_type pickedItemId)
    {
        PlayerAccountStorageComponent* accountStorageComponent = player.FindComponent<PlayerAccountStorageComponent>();
        if (!accountStorageComponent || accountStorageComponent->IsLoadPending())
        {
            return false;
        }

        PlayerItemComponent& itemComponent = player.GetItemComponent();
        const GameItem* pickedItem = itemComponent.GetPickedItem();

        if (pickedItem)
        {
            if (pickedItem->GetId() != pickedItemId)
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] invalid picked item id. player: {}, request: {}, real: {}",
                        GetName(), player.GetCId(), pickedItemId, pickedItem->GetId()));

                return false;
            }

            std::shared_ptr<GameItem> released = itemComponent.ReleaseItem(pickedItemId);
            assert(released);

            std::shared_ptr<GameItem> outLifted;
            if (!accountStorageComponent->LowerItem(std::move(released), page, x, y, outLifted))
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to lower item to account storage. player: {}, picked: {}, pos: [{}, {}, {}]",
                        GetName(), player.GetCId(), pickedItemId, page, x, y));

                return false;
            }

            player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, pickedItem->GetId(), pickedItem->GetType()));

            if (outLifted)
            {
                player.Defer(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *outLifted));

                [[maybe_unused]]
                const bool added = itemComponent.AddNewPickedItem(outLifted);
                assert(added);
            }
        }
        else
        {
            if (pickedItemId != game_entity_id_type(0))
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] invalid picked item id. player: {}, request: {}",
                        GetName(), player.GetCId(), pickedItemId));

                return false;
            }

            std::shared_ptr<GameItem> liftedItem = accountStorageComponent->LiftItem(page, x, y);
            if (!liftedItem)
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to lift item to account storage. player: {}, pos: [{}, {}, {}]",
                        GetName(), player.GetCId(), page, x, y));

                return false;
            }

            player.Defer(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *liftedItem));

            [[maybe_unused]]
            const bool added = itemComponent.AddNewPickedItem(liftedItem);
            assert(added);
        }

        return true;
    }

    auto ItemArchiveSystem::CreateNewGameItem(const ItemData& itemData, int32_t quantity) -> SharedPtrNotNull<GameItem>
    {
        assert(quantity > 0);

        auto item = std::make_shared<GameItem>(_serviceLocator.Get<GameEntityIdPublisher>(), itemData, quantity);
        item->AddComponent(std::make_unique<ItemPositionComponent>());

        return item;
    }

    void ItemArchiveSystem::AddDummyPacketForInventoryRefresh(GamePlayer& player)
    {
        player.Defer(ItemArchiveMessageCreator::CreateItemAddForRefresh(player, 12500060));
        player.Defer(ItemArchiveMessageCreator::CreateItemRemoveForRefresh(player));
    }

    void ItemArchiveSystem::SaveChanges(GamePlayer& player)
    {
        db::ItemTransaction transaction;

        if (PlayerItemComponent& playerItemComponent = player.GetItemComponent();
            playerItemComponent.HasItemLog())
        {
            playerItemComponent.FlushItemLogTo(transaction.logs);
        }

        if (PlayerAccountStorageComponent* accountStorageComponent = player.FindComponent<PlayerAccountStorageComponent>();
            accountStorageComponent && accountStorageComponent->HasItemLog())
        {
            accountStorageComponent->FlushItemLogTo(transaction.logs);
        }

        if (transaction.logs.empty())
        {
            return;
        }

        _serviceLocator.Get<GameRepositoryService>().Save(player, std::move(transaction));
    }

    void ItemArchiveSystem::LogHandleItemError(const char* func, const std::string& message) const
    {
        SUNLIGHT_LOG_ERROR(_serviceLocator,
            fmt::format("[{}] error. function: {}, message: {}",
                GetName(), func, message));
    }

    bool ItemArchiveSystem::IsExternItemTransactionRunning(const GamePlayer& player) const
    {
        const PlayerGroupComponent& groupComponent = player.GetGroupComponent();

        if (!groupComponent.HasGroup())
        {
            return false;
        }

        switch (groupComponent.GetGroupType())
        {
        case GameGroupType::Trade:
            return true;
        }

        return false;
    }

    bool ItemArchiveSystem::IsAllowed(const GamePlayer& player, ZoneMessageType itemMessage) const
    {
        if (IsExternItemTransactionRunning(player))
        {
            switch (itemMessage)
            {
            case ZoneMessageType::ITEMARCHIVE_LIFTITEM:
            case ZoneMessageType::ITEMARCHIVE_LOWERITEM:
                return true;
            default:;
            }

            return false;
        }

        return true;
    }
}

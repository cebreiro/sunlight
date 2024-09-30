#include "item_archive_system.h"

#include <boost/scope/scope_exit.hpp>

#include "sl/emulator/game/component/item_ownership_component.h"
#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/component/npc_item_shop_component.h"
#include "sl/emulator/game/component/player_account_storage_component.h"
#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/data/sox/item_etc.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_npc.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/item_archive_message_creator.h"
#include "sl/emulator/game/message/creator/npc_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/game_repository_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/game/zone/service/game_item_unique_id_publisher.h"
#include "sl/emulator/service/database/database_service.h"
#include "sl/emulator/service/gamedata/gamedata_provide_service.h"
#include "sl/emulator/service/gamedata/item/item_data.h"
#include "sl/emulator/service/gamedata/item/item_data_provider.h"
#include "sl/emulator/service/gamedata/shop/item_shop_data.h"

namespace sunlight
{
    ItemArchiveSystem::ItemArchiveSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void ItemArchiveSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<GameRepositorySystem>());
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerStatSystem>());
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

    void ItemArchiveSystem::Purchase(GamePlayer& player, GameNPC& npc, game_entity_id_type targetId, int32_t itemId, int32_t page, int32_t x, int32_t y, int32_t quantity)
    {
        PlayerItemComponent& playerItemComponent = player.GetItemComponent();

        NPCItemShopResult result = NPCItemShopResult::JustFail;

        do
        {
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

        if (!item->HasComponent<ItemPositionComponent>())
        {
            item->AddComponent<ItemPositionComponent>(std::make_unique<ItemPositionComponent>());
        }

        boost::scope::scope_exit exit([&]()
            {
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

        player.Send(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *item));

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

        if (!item->HasComponent<ItemPositionComponent>())
        {
            item->AddComponent<ItemPositionComponent>(std::make_unique<ItemPositionComponent>());
        }

        PlayerItemComponent& itemComponent = player.GetItemComponent();

        int32_t resultAddedCount = 0;

        boost::scope::scope_exit exit([&]()
            {
                addedQuantity = resultAddedCount;

                SaveChanges(player);
            });

        const ItemData& itemData = item->GetData();

        if (itemData.GetMaxOverlapCount() > 1)
        {
            bool fillItemQuantity = true;

            // exceptional case, client rules
            // if rules are not followed, client rejects to add overlap item quantity
            // so server-client item quantity synchronization is break
            // exceptional case 1
            if (itemData.IsAbleToUseQuickSlot())
            {
                while (item->GetQuantity() > 0)
                {
                    int32_t usedCount = 0;
                    std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>> result;

                    if (itemComponent.TryStackQuickSlotItem(itemData, item->GetQuantity(), usedCount, &result))
                    {
                        assert(usedCount > 0 && usedCount <= item->GetQuantity());

                        fillItemQuantity = false;

                        item->SetQuantity(item->GetQuantity() - usedCount);

                        resultAddedCount += usedCount;

                        for (const auto& [overlappedItem, overlappedQuantity] : result)
                        {
                            player.Send(ItemArchiveMessageCreator::CreateItemAdd(player, *overlappedItem, overlappedQuantity));
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }

            // exceptional case 2
            if (itemData.GetEtcData() && itemData.GetEtcData()->bulletType)
            {
                const GameItem* bulletItem = itemComponent.GetEquipmentItem(EquipmentPosition::Bullet);
                if (bulletItem)
                {
                    if (bulletItem->GetData().GetId() == itemData.GetId() &&
                        bulletItem->GetQuantity() < itemData.GetMaxOverlapCount())
                    {
                        fillItemQuantity = false;

                        const int32_t newQuantity = std::min(itemData.GetMaxOverlapCount(), bulletItem->GetQuantity() + item->GetQuantity());
                        const int32_t added = newQuantity - bulletItem->GetQuantity();

                        resultAddedCount += added;

                        item->SetQuantity(item->GetQuantity() - added);
                        assert(item->GetQuantity() >= 0);

                        [[maybe_unused]]
                        const bool success = itemComponent.SetItemQuantity(bulletItem->GetId(), newQuantity);
                        assert(success);

                        player.Send(ItemArchiveMessageCreator::CreateItemAdd(player, *bulletItem, added));
                    }
                }
            }

            if (fillItemQuantity)
            {
                while (item->GetQuantity() > 0)
                {
                    int32_t usedCount = 0;
                    std::vector<std::pair<PtrNotNull<const GameItem>, int32_t>> result;

                    if (itemComponent.TryStackItem(itemData.GetId(), item->GetQuantity(), usedCount, &result))
                    {
                        assert(usedCount > 0 && usedCount <= item->GetQuantity());

                        item->SetQuantity(item->GetQuantity() - usedCount);

                        resultAddedCount += usedCount;

                        for (const auto& [overlappedItem, overlappedQuantity] : result)
                        {
                            player.Send(ItemArchiveMessageCreator::CreateItemAdd(player, *overlappedItem, overlappedQuantity));
                        }
                    }
                    else
                    {
                        break;
                    }
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

        if (itemData.IsAbleToUseQuickSlot())
        {
            const std::optional<QuickSlotPosition>& pos = itemComponent.FindEmptyQuickSlotPosition();
            if (pos.has_value())
            {
                if (itemComponent.AddQuickSlotItem(item, &pos.value()))
                {
                    player.Send(ItemArchiveMessageCreator::CreateItemAdd(player, *item, item->GetQuantity()));

                    resultAddedCount += item->GetQuantity();

                    return true;
                }
            }
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

        player.Send(ItemArchiveMessageCreator::CreateItemAdd(player, *item, item->GetQuantity()));

        resultAddedCount += item->GetQuantity();

        return true;
    }

    bool ItemArchiveSystem::RemoveInventoryItem(GamePlayer& player, int32_t itemId, int32_t quantity)
    {
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

    void ItemArchiveSystem::OpenAccountStorage(GamePlayer& player)
    {
        PlayerAccountStorageComponent* accountStorageComponent = player.FindComponent<PlayerAccountStorageComponent>();
        if (!accountStorageComponent)
        {
            auto temp = std::make_unique<PlayerAccountStorageComponent>();
            assert(temp->IsLoadPending());

            accountStorageComponent = temp.get();
            player.AddComponent(std::move(temp));

            Get<GameRepositorySystem>().LoadAccountStorage(player,
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

        // TODO: update stat

        assert(itemComponent.HasItemLog());
        SaveChanges(player);

        player.Send(GamePlayerMessageCreator::CreatePlayerWeaponSwap(player, itemComponent.FindEquipmentItem(EquipmentPosition::Weapon1)));
        Get<EntityViewRangeSystem>().Broadcast(player, GamePlayerMessageCreator::CreateRemovePlayerWeaponChange(player), true);
    }

    void ItemArchiveSystem::OnCompleteLoadAccountStorage(int64_t cid, const db::dto::AccountStorage& dto)
    {
        GamePlayer* player = Get<SceneObjectSystem>().FindPlayerByCid(cid);
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

        boost::scope::scope_exit exit([&]()
            {
                if (success)
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
        default:
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] unhandled zone message. player: {}, type: {}, target: [{}, {}]",
                    GetName(), player.GetCId(), ToString(subType), targetId, ToString(targetType)));
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
                    bool result = itemComponent.SetItemQuantity(invenItemId, maxOverlapCount);
                    assert(result);

                    result = itemComponent.SetItemQuantity(picked->GetId(), sum - maxOverlapCount);
                    assert(result);
                }
                else
                {
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
                    bool result = itemComponent.SetItemQuantity(quickSlotItemId, maxOverlapCount);
                    assert(result);

                    result = itemComponent.SetItemQuantity(picked->GetId(), sum - maxOverlapCount);
                    assert(result);
                }
                else
                {
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


        if (position == EquipmentPosition::Hat)
        {
            const PlayerAppearanceComponent& appearanceComponent = player.GetAppearanceComponent();

            Get<EntityViewRangeSystem>().Broadcast(player,
                GamePlayerMessageCreator::CreatePlayerEquipmentChange(player, position, appearanceComponent.GetHair(), appearanceComponent.GetHairColor()), false);
        }
        else
        {
            Get<EntityViewRangeSystem>().Broadcast(player,
                GamePlayerMessageCreator::CreatePlayerEquipmentChange(player, position, 0, 0), false);
        }

        Get<PlayerStatSystem>().RemoveItemStat(player, *itemComponent.GetEquipmentItem(position));

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

            PlayerStatSystem& playerStatUpdateSystem = Get<PlayerStatSystem>();

            playerStatUpdateSystem.RemoveItemStat(player, *itemComponent.GetPickedItem());
            playerStatUpdateSystem.AddItemStat(player, *itemComponent.GetEquipmentItem(position));
        }
        else
        {
            if (!itemComponent.LowerPickedItemTo(position))
            {
                LogHandleItemError(__FUNCTION__, "fail to lower picked item to pos");

                return false;
            }

            Get<PlayerStatSystem>().AddItemStat(player, *itemComponent.GetEquipmentItem(position));
        }

        const GameItem* equipItem = itemComponent.GetEquipmentItem(position);
        assert(equipItem);

        Get<EntityViewRangeSystem>().Broadcast(player,
            GamePlayerMessageCreator::CreatePlayerEquipmentChange(player, position,
                equipItem->GetData().GetModelId(), equipItem->GetData().GetModelColor()), false);

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
        ownershipComponent.SetEndTimePoint(GameTimeService::Now() + std::chrono::seconds(30));

        Eigen::Vector2f destPosition = player.GetSceneObjectComponent().GetPosition();
        destPosition.x() += 30.f;
        destPosition.y() += 30.f;

        Get<SceneObjectSystem>().SpawnItem(std::move(pickedItem),
            player.GetSceneObjectComponent().GetPosition(), destPosition);

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

            std::shared_ptr<GameItem> shared = itemComponent.FindItemShared(pickedItemId);
            assert(shared);

            std::shared_ptr<GameItem> outLifted;
            if (!accountStorageComponent->LowerItem(std::move(shared), page, x, y, outLifted))
            {
                SUNLIGHT_LOG_ERROR(_serviceLocator,
                    fmt::format("[{}] fail to lower item to account storage. player: {}, picked: {}, pos: [{}, {}, {}]",
                        GetName(), player.GetCId(), pickedItemId, page, x, y));

                return false;
            }

            player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, pickedItem->GetId(), pickedItem->GetType()));

            [[maybe_unused]]
            const bool removed = itemComponent.RemovePickedItem();
            assert(removed);

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

        Get<GameRepositorySystem>().Save(player, std::move(transaction));
    }

    void ItemArchiveSystem::LogHandleItemError(const char* func, const std::string& message) const
    {
        SUNLIGHT_LOG_ERROR(_serviceLocator,
            fmt::format("[{}] error. function: {}, message: {}",
                GetName(), func, message));
    }
}

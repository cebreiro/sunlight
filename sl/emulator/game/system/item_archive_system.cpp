#include "item_archive_system.h"

#include <boost/scope/scope_exit.hpp>

#include "sl/emulator/game/component/item_ownership_component.h"
#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/data/sox/item_etc.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/message/creator/item_archive_message_creator.h"
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

                player.Defer(ItemArchiveMessageCreator::CreateArchiveResult(player, success, subType));
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
        case ZoneMessageType::ITEMARCHIVE_ADDITEM:
        case ZoneMessageType::ITEMARCHIVE_USEITEM:
        case ZoneMessageType::ITEMARCHIVE_ADD_SUB_GOLD:
        case ZoneMessageType::ITEMARCHIVE_REMOVEITEM:
        case ZoneMessageType::ITEMARCHIVE_RESULT:
        case ZoneMessageType::ITEMARCHIVE_LORDITEM:
        case ZoneMessageType::ITEMARCHIVE_ALLINVEN:
        case ZoneMessageType::ITEMARCHIVE_DROPMONEY:
        case ZoneMessageType::ITEMARCHIVE_DECREASEITEM:
        case ZoneMessageType::ITEMARCHIVE_ADDINVENITEM:
        case ZoneMessageType::ITEMARCHIVE_REALIGNITEM:
        case ZoneMessageType::ITEMARCHIVE_LOCAL_USE_ITEM:
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

    auto ItemArchiveSystem::CreateNewGameItem(const ItemData& itemData, int32_t quantity) -> SharedPtrNotNull<GameItem>
    {
        assert(quantity > 0);

        auto item = std::make_shared<GameItem>(_serviceLocator.Get<GameEntityIdPublisher>(), itemData, quantity);
        item->AddComponent(std::make_unique<ItemPositionComponent>());

        return item;
    }

    void ItemArchiveSystem::SaveChanges(GamePlayer& player)
    {
        PlayerItemComponent& playerItemComponent = player.GetItemComponent();

        if (!playerItemComponent.HasItemLog())
        {
            return;
        }
            
        db::ItemTransaction transaction;
        playerItemComponent.FlushItemLogTo(transaction.logs);

        Get<GameRepositorySystem>().Save(player, std::move(transaction));
    }

    void ItemArchiveSystem::LogHandleItemError(const char* func, const std::string& message) const
    {
        SUNLIGHT_LOG_ERROR(_serviceLocator,
            fmt::format("[{}] error. function: {}, message: {}",
                GetName(), func, message));
    }
}

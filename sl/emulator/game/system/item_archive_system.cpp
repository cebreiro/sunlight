#include "item_archive_system.h"

#include <boost/scope/scope_exit.hpp>

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/item_archive_message_creator.h"
#include "sl/emulator/game/system/player_stat_update_system.h"
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
        Add(stage.Get<PlayerStatUpdateSystem>());
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

    bool ItemArchiveSystem::AddItem(GamePlayer& player, int32_t itemId, int32_t quantity, int32_t& addedQuantity)
    {
        const ItemDataProvider& itemDataProvider = _serviceLocator.Get<GameDataProvideService>().GetItemDataProvider();
        const ItemData* itemData = itemDataProvider.Find(itemId);
        if (!itemData)
        {
            return false;
        }

        return AddItem(player, CreateNewGameItem(*itemData, quantity), addedQuantity);
    }

    bool ItemArchiveSystem::AddItem(GamePlayer& player, SharedPtrNotNull<GameItem> item, int32_t& addedQuantity)
    {
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
            while (item->GetQuantity() > 0)
            {
                int32_t usedCount = 0;

                if (itemComponent.TryStackOverlapItem(itemData.GetId(), item->GetQuantity(), usedCount))
                {
                    assert(usedCount > 0 && usedCount <= item->GetQuantity());

                    item->SetQuantity(item->GetQuantity() - usedCount);

                    resultAddedCount += usedCount;
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

        const std::optional<InventoryPosition>& pos = itemComponent.FindEmptyInventoryPosition(itemData.GetWidth(), itemData.GetHeight());
        if (!pos.has_value())
        {
            return false;
        }

        item->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());

        if (!itemComponent.AddInventoryItem(item, &pos.value()))
        {
            return false;
        }

        resultAddedCount += item->GetQuantity();

        player.Send(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *item));

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

                player.Send(ItemArchiveMessageCreator::CreateArchiveResult(player, success, subType));
            });

        switch (subType)
        {
        case ZoneMessageType::ITEMARCHIVE_INIT:
        {
            player.Send(ItemArchiveMessageCreator::CreateInit(player));
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
        case ZoneMessageType::ITEMARCHIVE_ADDITEM:
        
        case ZoneMessageType::ITEMARCHIVE_LOWERITEM_QI:
        case ZoneMessageType::ITEMARCHIVE_DROPITEM:
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

        const GameItem* targetItem = itemComponent.FindInventoryItem(targetItemId);
        if (!targetItem)
        {
            LogHandleItemError(__FUNCTION__, fmt::format("invalid request. player: {}, item_id: {}, quantity: {}",
                player.GetCId(), targetItemId, quantity));

            return false;
        }

        if (quantity < 0 || targetItem->GetQuantity() == quantity)
        {
            if (!itemComponent.LiftInventoryItem(targetItemId))
            {
                LogHandleItemError(__FUNCTION__, fmt::format("fail to lift inventory item. item_id: {}, quantity: {}",
                    targetItemId, quantity));

                return false;
            }
        }
        else
        {
            auto newPickedItem = CreateNewGameItem(targetItem->GetData(), quantity);
            newPickedItem->SetUId(_serviceLocator.Get<GameItemUniqueIdPublisher>().Publish());

            bool success = itemComponent.DecreaseItemQuantity(targetItemId, quantity);
            assert(success);

            success = itemComponent.AddNewPickedItem(std::move(newPickedItem));
            assert(success);

            player.Send(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *itemComponent.GetPickedItem()));
            player.Send(ItemArchiveMessageCreator::CreateItemDecrease(player, *targetItem, quantity));
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

    bool ItemArchiveSystem::HandleLiftEquipment(GamePlayer& player, EquipmentPosition position)
    {
        PlayerItemComponent& itemComponent = player.GetItemComponent();

        if (!player.GetItemComponent().LiftEquipItem(position))
        {
            return false;
        }

        Get<PlayerStatUpdateSystem>().RemoveItemStat(player, *itemComponent.GetEquipmentItem(position));

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

            PlayerStatUpdateSystem& playerStatUpdateSystem = Get<PlayerStatUpdateSystem>();

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

            Get<PlayerStatUpdateSystem>().AddItemStat(player, *itemComponent.GetEquipmentItem(position));
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
        PlayerItemComponent& playerItemComponent = player.GetItemComponent();

        if (!playerItemComponent.HasItemLog())
        {
            return;
        }
            
        db::ItemTransaction transaction;
        playerItemComponent.FlushItemLogTo(transaction.logs);

        execution::IExecutor* current = ExecutionContext::GetExecutor();
        assert(current);

        _serviceLocator.Get<DatabaseService>().StartTransaction(std::move(transaction))
            .Then(*current, [self = shared_from_this(), cid = player.GetCId()](bool success)
                {
                    if (!success)
                    {
                        self->HandleDatabaseError(cid);
                    }
                });
    }

    void ItemArchiveSystem::HandleDatabaseError(int64_t cid)
    {
        (void)cid;

        // TODO: disconnect player from server
    }

    void ItemArchiveSystem::LogHandleItemError(const char* func, const std::string& message) const
    {
        SUNLIGHT_LOG_ERROR(_serviceLocator,
            fmt::format("[{}] error. function: {}, message: {}",
                GetName(), func, message));
    }
}

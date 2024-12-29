#include "item_trade_system.h"

#include "sl/generator/game/component/item_position_component.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/player_item_trade_component.h"
#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/item_archive_message_creator.h"
#include "sl/generator/game/message/creator/item_trade_message_creator.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/game_repository_service.h"
#include "sl/generator/service/database/transaction/item/item_transaction.h"
#include "sl/generator/service/gamedata/item/item_data.h"

namespace sunlight
{
    ItemTradeSystem::ItemTradeSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void ItemTradeSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    auto ItemTradeSystem::GetName() const -> std::string_view
    {
        return "item_trade_system";
    }

    auto ItemTradeSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<ItemTradeSystem>();
    }

    void ItemTradeSystem::Start(GamePlayer& player)
    {
        assert(!player.HasComponent<PlayerItemTradeComponent>());

        auto itemTradeComponent = std::make_unique<PlayerItemTradeComponent>();

        for (const GameItem& item : player.GetItemComponent().GetItemRange())
        {
            const ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();
            const ItemPositionType posType = positionComponent.GetPositionType();

            switch (posType)
            {
            case ItemPositionType::Pick:
            case ItemPositionType::Inventory:
                break;
            case ItemPositionType::Equipment:
            case ItemPositionType::QuickSlot:
            case ItemPositionType::Count:
            default:
                continue;
            }

            itemTradeComponent->AddOriginState(*item.GetUId(), ItemTradeOriginState{
                .id = item.GetId(),
                .type = item.GetType(),
                .data = &item.GetData(),
                .quantity = item.GetQuantity(),
                .positionType = posType,
                .page = positionComponent.GetPage(),
                .x = positionComponent.GetX(),
                .y = positionComponent.GetY(),
            });
        }

        player.AddComponent(std::move(itemTradeComponent));
    }

    bool ItemTradeSystem::Rollback(GamePlayer& player)
    {
        assert(player.HasComponent<PlayerItemTradeComponent>());

        PlayerItemComponent& itemComponent = player.GetItemComponent();
        PlayerItemTradeComponent& tradeComponent = player.GetComponent<PlayerItemTradeComponent>();

        std::unordered_map<int64_t, std::pair<SharedPtrNotNull<GameItem>, PtrNotNull<const ItemTradeOriginState>>> recovers;
        for (SharedPtrNotNull<GameItem>& tradeItem : tradeComponent.GetItems() | std::views::values | std::views::elements<0>)
        {
            const int64_t uid = *tradeItem->GetUId();

            if (const ItemTradeOriginState* originState = tradeComponent.FindOriginState(uid); originState)
            {
                assert(!recovers.contains(uid));

                tradeItem->SetQuantity(originState->quantity);

                recovers[uid] = std::make_pair(std::move(tradeItem), originState);
            }
        }

        std::vector<Buffer> packets;
        bool buildResult = true;

        db::ItemTransaction transaction;
        itemComponent.FlushItemLogTo(transaction.logs);

        for (const db::ItemLog& element : transaction.logs)
        {
            buildResult &= std::visit([&]<typename T>(const T & log) -> bool
            {
                if constexpr (std::is_same_v<T, db::ItemLogAdd> ||
                    std::is_same_v<T, db::ItemLogRemove> ||
                    std::is_same_v<T, db::ItemLogUpdatePosition> ||
                    std::is_same_v<T, db::ItemLogUpdateQuantity>)
                {
                    if (recovers.contains(log.id))
                    {
                        return true;
                    }

                    // remove all change items
                    std::shared_ptr<GameItem> item = itemComponent.ReleaseItemByUId(log.id);
                    if (item)
                    {
                        packets.emplace_back(ItemArchiveMessageCreator::CreateItemRemove(player, item->GetId(), item->GetType()));
                    }

                    if (const ItemTradeOriginState* originState = tradeComponent.FindOriginState(log.id); originState)
                    {
                        if (item)
                        {
                            item->SetQuantity(originState->quantity);
                        }
                        else
                        {
                            packets.emplace_back(ItemArchiveMessageCreator::CreateItemRemove(player, originState->id, originState->type));

                            item = std::make_shared<GameItem>(_serviceLocator.Get<GameEntityIdPublisher>(), *originState->data, originState->quantity);
                            item->SetUId(log.id);
                            item->AddComponent(std::make_unique<ItemPositionComponent>());
                        }

                        recovers.try_emplace(log.id, std::make_pair(std::move(item), originState));
                    }

                    return true;
                }
                else
                {
                    SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                        fmt::format("invalid transaction type found. rollback result may be incorrect. player: {}, transaction: {}",
                            player.GetCId(), ToString(log.type)));

                    return false;
                }

            }, element);
        }

        if (!buildResult)
        {
            return false;
        }

        for (auto& [uid, pair] : recovers)
        {
            SharedPtrNotNull<GameItem>& item = pair.first;
            PtrNotNull<const ItemTradeOriginState> originState = pair.second;

            switch (originState->positionType)
            {
            case ItemPositionType::Inventory:
            {
                if (!itemComponent.IsEmpty(originState->page, ItemSlotRange{
                    .x = originState->x,
                    .y = originState->y,
                    .xSize = item->GetData().GetWidth(),
                    .ySize = item->GetData().GetHeight(),
                    }))
                {
                    SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                        fmt::format("item's origin position is not empty. rollback result may be incorrect. player: {}, item_uid: {}",
                            player.GetCId(), uid));

                    return false;
                }

                GameItem* itemPrt = item.get();

                const InventoryPosition hint{
                    .page = originState->page,
                    .x = originState->x,
                    .y = originState->y
                };

                [[maybe_unused]]
                const bool added = itemComponent.AddInventoryItem(std::move(item), &hint);
                assert(added);

                packets.emplace_back(ItemArchiveMessageCreator::CreateInventoryItemAdd(player, *itemPrt));
            }
            break;
            case ItemPositionType::Pick:
            {
                if (itemComponent.GetPickedItem())
                {
                    SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                        fmt::format("already exists picked item. rollback result may be incorrect. player: {}, item_uid: {}",
                            player.GetCId(), uid));

                    return false;
                }

                packets.emplace_back(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *item));

                itemComponent.AddNewPickedItem(std::move(item));
            }
            break;
            case ItemPositionType::Equipment:
            case ItemPositionType::QuickSlot:
            case ItemPositionType::Count:
            default:
                assert(false);
            }
        }

        // remove rollback log
        itemComponent.FlushItemLogTo(transaction.logs);

        if (!packets.empty())
        {
            for (Buffer& buffer : packets)
            {
                player.Defer(std::move(buffer));
            }

            // to refresh client inventory UI
            player.Defer(ItemArchiveMessageCreator::CreateItemAddForRefresh(player, 12500060));
            player.Defer(ItemArchiveMessageCreator::CreateItemRemoveForRefresh(player));

            player.FlushDeferred();
        }

        player.RemoveComponent<PlayerItemTradeComponent>();

        return true;
    }

    bool ItemTradeSystem::Commit(GamePlayer& host, GamePlayer& guest)
    {
        assert(host.HasComponent<PlayerItemTradeComponent>());
        assert(guest.HasComponent<PlayerItemTradeComponent>());

        PlayerItemTradeComponent& hostTradeComponent = host.GetComponent<PlayerItemTradeComponent>();
        PlayerItemTradeComponent& guestTradeComponent = guest.GetComponent<PlayerItemTradeComponent>();
        PlayerItemComponent& hostItemComponent = host.GetItemComponent();
        PlayerItemComponent& guestItemComponent = guest.GetItemComponent();

        if (hostItemComponent.GetGold() < hostTradeComponent.GetGold() ||
            guestItemComponent.GetGold() < guestTradeComponent.GetGold())
        {
            return false;
        }

        using add_position_container_type = std::unordered_map<game_entity_id_type, InventoryPosition>;

        const auto buildPositionContainer = [](add_position_container_type& result,
            std::vector<ItemSlotStorageBase<bool>> masks, const PlayerItemTradeComponent& tradeComponent) -> bool
            {
                assert(!masks.empty());

                int64_t index = 0;

                for (const auto& [item, range] : tradeComponent.GetItems() | std::views::values)
                {
                    std::optional<InventoryPosition> position = std::nullopt;

                    while (index < std::ssize(masks))
                    {
                        ItemSlotStorageBase<bool>& mask = masks[index];

                        if (const std::optional<std::pair<int32_t, int32_t>> opt = mask.FindEmpty(range.xSize, range.ySize);
                            opt.has_value())
                        {
                            position = InventoryPosition{
                                .page = static_cast<int8_t>(index),
                                .x = static_cast<int8_t>(opt->first),
                                .y = static_cast<int8_t>(opt->second),
                            };

                            mask.Set(true, ItemSlotRange{
                                .x = static_cast<int8_t>(opt->first),
                                .y = static_cast<int8_t>(opt->second),
                                .xSize = range.xSize,
                                .ySize = range.ySize,
                                });

                            break;
                        }

                        ++index;
                    }

                    if (!position.has_value())
                    {
                        return false;
                    }

                    [[maybe_unused]]
                    const bool inserted = result.try_emplace(item->GetId(), *position).second;
                    assert(inserted);
                }

                return true;
            };

        add_position_container_type hostInventoryAddPositions;
        if (!buildPositionContainer(hostInventoryAddPositions, hostItemComponent.GetInventoryMasks(), guestTradeComponent))
        {
            return false;
        }

        add_position_container_type guestInventoryAddPositions;
        if (!buildPositionContainer(guestInventoryAddPositions, guestItemComponent.GetInventoryMasks(), hostTradeComponent))
        {
            return false;
        }

        std::vector<Buffer> hostSyncPackets;
        hostSyncPackets.reserve(hostInventoryAddPositions.size());

        std::vector<Buffer> guestSyncPackets;
        guestSyncPackets.reserve(guestInventoryAddPositions.size());

        db::ItemTransaction transaction;
        hostItemComponent.FlushItemLogTo(transaction.logs);
        guestItemComponent.FlushItemLogTo(transaction.logs);

        [[maybe_unused]]
        bool added = false;

        for (const auto& [itemId, position] : hostInventoryAddPositions)
        {
            std::shared_ptr<GameItem> tradeItem = guestTradeComponent.Release(itemId);
            assert(tradeItem);

            GameItem* tradeItemPtr = tradeItem.get();

            added = hostItemComponent.AddInventoryItem(std::move(tradeItem), &position);
            assert(added);

            hostSyncPackets.emplace_back(ItemArchiveMessageCreator::CreateInventoryItemAdd(host, *tradeItemPtr));
        }

        for (const auto& [itemId, position] : guestInventoryAddPositions)
        {
            std::shared_ptr<GameItem> tradeItem = hostTradeComponent.Release(itemId);
            assert(tradeItem);

            GameItem* tradeItemPtr = tradeItem.get();

            added = guestItemComponent.AddInventoryItem(std::move(tradeItem), &position);
            assert(added);

            guestSyncPackets.emplace_back(ItemArchiveMessageCreator::CreateInventoryItemAdd(guest, *tradeItemPtr));
        }

        if (hostTradeComponent.GetGold() > 0 || guestTradeComponent.GetGold() > 0)
        {
            const int32_t hostAddGold = guestTradeComponent.GetGold() - hostTradeComponent.GetGold();
            const int32_t guestAddGold = -hostAddGold;

            hostItemComponent.AddOrSubGold(hostAddGold);
            guestItemComponent.AddOrSubGold(guestAddGold);

            host.Defer(ItemArchiveMessageCreator::CreateGoldAddOrSub(host, hostAddGold));
            guest.Defer(ItemArchiveMessageCreator::CreateGoldAddOrSub(guest, guestAddGold));
        }

        hostItemComponent.FlushItemLogTo(transaction.logs);
        guestItemComponent.FlushItemLogTo(transaction.logs);

        _serviceLocator.Get<GameRepositoryService>().Save(host, guest, std::move(transaction));

        for (Buffer& buffer : hostSyncPackets)
        {
            host.Defer(std::move(buffer));
        }

        for (Buffer& buffer : guestSyncPackets)
        {
            guest.Defer(std::move(buffer));
        }

        host.FlushDeferred();
        guest.FlushDeferred();

        host.RemoveComponent<PlayerItemTradeComponent>();
        guest.RemoveComponent<PlayerItemTradeComponent>();

        return true;
    }

    bool ItemTradeSystem::LiftItem(GameGroup& group, GamePlayer& player, game_entity_id_type itemId)
    {
        assert(player.HasComponent<PlayerItemTradeComponent>());

        PlayerItemComponent& itemComponent = player.GetItemComponent();
        if (itemComponent.GetPickedItem())
        {
            return false;
        }

        PlayerItemTradeComponent& itemTradeComponent = player.GetComponent<PlayerItemTradeComponent>();

        std::shared_ptr<GameItem> item = itemTradeComponent.LiftItem(itemId);
        if (!item)
        {
            return false;
        }

        itemComponent.AddNewPickedItem(item);

        player.Defer(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *item));
        player.Defer(ItemTradeMessageCreator::CreateLiftItemResult(group.GetId()));
        player.FlushDeferred();

        if (group.GetMemberCount() > 1)
        {
            group.Broadcast(ItemTradeMessageCreator::CreateLiftItem(group.GetId(), item->GetId(), item->GetType()), player.GetId());
        }

        return true;
    }

    bool ItemTradeSystem::LowerItem(GameGroup& group, GamePlayer& player, int32_t x, int32_t y, game_entity_id_type itemId)
    {
        assert(player.HasComponent<PlayerItemTradeComponent>());

        PlayerItemTradeComponent& itemTradeComponent = player.GetComponent<PlayerItemTradeComponent>();
        if (itemTradeComponent.Contains(itemId))
        {
            return false;
        }

        PlayerItemComponent& itemComponent = player.GetItemComponent();

        if (const GameItem* pickedItem = itemComponent.GetPickedItem();
            !pickedItem || pickedItem->GetId() != itemId)
        {
            return false;
        }

        std::shared_ptr<GameItem> item = itemComponent.ReleaseItem(itemId);
        if (!item)
        {
            return false;
        }

        std::shared_ptr<GameItem> liftedItem;

        if (!itemTradeComponent.LowerItem(item, ItemSlotRange{
            .x = x,
            .y = y,
            .xSize = item->GetData().GetWidth(),
            .ySize = item->GetData().GetHeight(),
            },
            liftedItem))
        {
            return false;
        }

        player.Defer(ItemArchiveMessageCreator::CreateItemRemove(player, item->GetId(), item->GetType()));

        if (liftedItem)
        {
            player.Defer(ItemArchiveMessageCreator::CreateNewPickedItemAdd(player, *liftedItem));

            itemComponent.AddNewPickedItem(std::move(liftedItem));
        }

        player.Defer(ItemTradeMessageCreator::CreateLowerItemResult(group.GetId()));
        player.FlushDeferred();

        if (group.GetMemberCount() > 1)
        {
            group.Broadcast(ItemTradeMessageCreator::CreateLowerItem(group.GetId(), x, y, *item), player.GetId());
        }

        return true;
    }

    bool ItemTradeSystem::ChangeGold(GameGroup& group, GamePlayer& player, int32_t gold)
    {
        assert(player.HasComponent<PlayerItemTradeComponent>());

        const PlayerItemComponent& itemComponent = player.GetItemComponent();
        PlayerItemTradeComponent& itemTradeComponent = player.GetComponent<PlayerItemTradeComponent>();

        if (itemComponent.GetGold() < gold)
        {
            return false;
        }

        itemTradeComponent.SetGold(gold);

        player.Send(ItemTradeMessageCreator::CreateGoldChangeResult(group.GetId()));

        if (group.GetMemberCount() > 1)
        {
            group.Broadcast(ItemTradeMessageCreator::CreateGoldChange(group.GetId(), gold), player.GetId());
        }

        return true;
    }
}

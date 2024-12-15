#include "street_vendor_purchase_message_handler.h"

#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/street_vendor_host_component.h"
#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/contents/group/street_vendor_purchase_result.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_stored_item.h"
#include "sl/generator/game/message/creator/street_vendor_message_creator.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void StreetVendorPurchaseMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        const int32_t page = reader.Read<int32_t>();

        StreetVendorPurchaseResult result = StreetVendorPurchaseResult::AlreadySold;
        bool soldOut = false;

        do
        {
            if (group.IsHost(player))
            {
                break;
            }

            GamePlayer& host = group.GetHost();
            StreetVendorHostComponent& hostComponent = host.GetComponent<StreetVendorHostComponent>();

            if (!hostComponent.IsValid(page))
            {
                break;
            }

            const int32_t price = hostComponent.GetItemPrice(page);

            PlayerItemComponent& playerItemComponent = player.GetItemComponent();
            if (playerItemComponent.GetGold() < price)
            {
                result = StreetVendorPurchaseResult::LockOfMoney;

                break;
            }

            PlayerItemComponent& hostItemComponent = host.GetItemComponent();

            const GameItem* targetItem = hostItemComponent.GetVendorSaleItem(page);
            if (!targetItem)
            {
                break;
            }

            if (!system.Get<ItemArchiveSystem>().PurchaseStreetVendorItem(host, player, targetItem->GetId(), price))
            {
                result = StreetVendorPurchaseResult::LockOfSpace;

                break;
            }

            result = StreetVendorPurchaseResult::Success;

            soldOut = hostItemComponent.GetVendorSaleItemSize() <= 0;

            if (const std::optional<game_entity_id_type> storedItemId = hostComponent.GetStoredItem(page);
                storedItemId.has_value())
            {
                // accessed by clicking field displayed 'stored item'

                const GameEntity* storedItem = system.Get<SceneObjectSystem>().FindEntity(GameStoredItem::TYPE, *storedItemId);
                if (!storedItem)
                {
                    assert(false);

                    break;
                }

                const int32_t offset = storedItem->Cast<GameStoredItem>()->GetFieldOffset();

                for (const auto& [saleItemPage, saleItem] : hostItemComponent.GetVendorSaleItems())
                {
                    if (hostComponent.IsDisplayedItemPage(saleItemPage))
                    {
                        continue;
                    }

                    system.SpawnStoredItem(group, host, saleItem, saleItemPage, hostComponent.GetItemPrice(saleItemPage), offset);

                    break;
                }

                system.Get<SceneObjectSystem>().RemoveStoredItem(*storedItemId);
                hostComponent.RemoveStoredItem(page);
            }

            player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), page / 2 * 2, group.GetHost()));

        } while (false);

        player.Send(StreetVendorMessageCreator::CreateItemPurchaseResult(group.GetId(), result));

        if (soldOut)
        {
            group.GetHost().Send(StreetVendorMessageCreator::CreateVendorItemSoldOut(group.GetId()));

            system.ProcessHostExit(group, group.GetHost());
        }
    }
}

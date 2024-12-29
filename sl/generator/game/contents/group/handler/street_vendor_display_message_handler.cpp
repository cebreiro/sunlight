#include "street_vendor_display_message_handler.h"

#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/street_vendor_host_component.h"
#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/contents/group/street_vendor_start_result.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/street_vendor_message_creator.h"
#include "sl/generator/game/system/player_group_system.h"

namespace sunlight
{
    void StreetVendorDisplayMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        (void)reader;

        const PlayerItemComponent& itemComponent = player.GetItemComponent();
        StreetVendorHostComponent& hostComponent = player.GetComponent<StreetVendorHostComponent>();

        std::array<std::tuple<const GameItem*, int32_t, int32_t>, GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE> storedItemData = {};
        int32_t k = 0;

        int32_t nullItemCount = 0;

        for (int32_t i = 0; i < GameConstant::MAX_STREET_VENDOR_PAGE_SIZE; ++i)
        {
            if (const GameItem* saleItem = itemComponent.GetVendorSaleItem(i); saleItem)
            {
                const int32_t price = hostComponent.GetItemPrice(i);
                if (price <= 0)
                {
                    player.Send(StreetVendorMessageCreator::CreateOpenResult(group.GetId(), StreetVendorStartResult::NoSaleItemPrice));

                    return;
                }

                if (k < std::ssize(storedItemData))
                {
                    storedItemData[k++] = std::tuple{ saleItem, i, price };
                }
            }
            else
            {
                ++nullItemCount;
            }
        }

        if (nullItemCount >= GameConstant::MAX_STREET_VENDOR_PAGE_SIZE)
        {
            player.Send(StreetVendorMessageCreator::CreateOpenResult(group.GetId(), StreetVendorStartResult::NoSaleItem));

            return;
        }

        for (int32_t i = 0; i < GameConstant::MAX_STREET_VENDOR_STORED_ITEM_SIZE; ++i)
        {
            const auto [saleItem, page, price] = storedItemData[i];
            if (saleItem)
            {
                system.SpawnStoredItem(group, player, *saleItem, page, price, i);
            }
            else
            {
                break;
            }
        }

        hostComponent.SetOpen(true);

        player.Send(StreetVendorMessageCreator::CreateOpenResult(group.GetId(), StreetVendorStartResult::Success));
    }
}

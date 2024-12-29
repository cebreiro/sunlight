#include "street_vendor_change_price_message_handler.h"

#include "sl/generator/game/component/street_vendor_host_component.h"
#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/street_vendor_message_creator.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void StreetVendorChangePriceMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        const int32_t index = reader.Read<int32_t>();
        const int32_t price = reader.Read<int32_t>();

        StreetVendorHostComponent& hostComponent = player.GetComponent<StreetVendorHostComponent>();

        if (hostComponent.IsOpen())
        {
            player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), index / 2 * 2, player));

            return;
        }

        if (!hostComponent.IsValid(index))
        {
            SUNLIGHT_LOG_ERROR(system.GetServiceLocator(),
                fmt::format("fail to configure item price. invalid index. player: {}, group: {}, index: {}, price: {}",
                    player.GetCId(), group.GetId(), index, price));

            return;
        }

        hostComponent.SetItemPrice(index, price);

        player.Send(StreetVendorMessageCreator::CreateItemPriceChange(group.GetId(), index, price));
    }
}

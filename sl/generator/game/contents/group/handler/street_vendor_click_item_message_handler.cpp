#include "street_vendor_click_item_message_handler.h"

#include "sl/generator/game/component/street_vendor_host_component.h"
#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/street_vendor_message_creator.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void StreetVendorClickItemMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        const int32_t index = reader.Read<int32_t>();

        if (player.GetComponent<StreetVendorHostComponent>().IsOpen())
        {
            player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), index / 2 * 2, player));

            return;
        }

        if (!system.Get<ItemArchiveSystem>().OnVendorSaleStorageClick(player, index))
        {
            return;
        }

        const int32_t page = (index / 2) * 2;

        player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), page, player));
    }
}

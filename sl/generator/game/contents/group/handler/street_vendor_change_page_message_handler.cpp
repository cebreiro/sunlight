#include "street_vendor_change_page_message_handler.h"

#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/street_vendor_message_creator.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void StreetVendorChangePageMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        (void)system;

        const int32_t page = reader.Read<int32_t>();

        if (group.IsHost(player))
        {
            player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), (page / 2) * 2, player));
        }
        else
        {
            player.Send(StreetVendorMessageCreator::CreatePageItemDisplay(group.GetId(), (page / 2) * 2, group.GetHost()));
        }
    }
}

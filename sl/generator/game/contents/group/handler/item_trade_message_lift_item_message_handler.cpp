#include "item_trade_message_lift_item_message_handler.h"

#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/item_trade_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void ItemTradeLiftItemMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        const auto [targetItemId, targetItemType] = reader.ReadInt64();

        if (!system.Get<ItemTradeSystem>().LiftItem(group, player, game_entity_id_type(targetItemId)))
        {
            player.GetClient().Disconnect();
        }
    }
}

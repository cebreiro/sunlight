#include "item_trade_message_lift_item_message_handler.h"

#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/item_trade_system.h"
#include "sl/emulator/game/system/player_group_system.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"

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

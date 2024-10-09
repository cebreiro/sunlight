#include "item_trade_message_lower_item_message_handler.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/item_trade_system.h"
#include "sl/emulator/game/system/player_group_system.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void ItemTradeLowerItemMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        const int32_t x = reader.Read<int32_t>();
        const int32_t y = reader.Read<int32_t>();
        const auto [targetItemId, targetItemType] = reader.ReadInt64();

        if (!system.Get<ItemTradeSystem>().LowerItem(group, player, x, y, game_entity_id_type(targetItemId)))
        {
            player.GetClient().Disconnect();
        }
    }
}

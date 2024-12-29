#include "item_trade_message_change_gold_message_handler.h"

#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/item_trade_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void ItemTradeChangeGoldMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        const int32_t gold = reader.Read<int32_t>();

        if (!system.Get<ItemTradeSystem>().ChangeGold(group, player, gold))
        {
            player.GetClient().Disconnect();
        }
    }
}

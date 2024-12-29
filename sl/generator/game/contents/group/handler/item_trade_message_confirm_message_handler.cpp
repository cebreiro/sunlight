#include "item_trade_message_confirm_message_handler.h"

#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/item_trade_message_creator.h"
#include "sl/generator/game/system/item_trade_system.h"
#include "sl/generator/game/system/player_group_system.h"

namespace sunlight
{
    void ItemTradeConfirmMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        (void)reader;

        group.Broadcast(ItemTradeMessageCreator::CreateTradeConfirm(group.GetId()), player.GetId());

        group.AddTradeConfirmPlayer(player);

        if (group.GetTradeConfirmPlayerCount() >= 2)
        {
            GamePlayer& host = group.GetHost();
            GamePlayer& guest = *group.GetGuests()[0];

            if (system.Get<ItemTradeSystem>().Commit(host, guest))
            {
                group.Broadcast(ItemTradeMessageCreator::CreateTradeSuccess(group.GetId()), std::nullopt);
            }
            else
            {
                system.ProcessTradeFail(group, host);
                system.ProcessTradeFail(group, guest);
            }

            system.ProcessTradeComplete(group, host, guest);
        }
    }
}

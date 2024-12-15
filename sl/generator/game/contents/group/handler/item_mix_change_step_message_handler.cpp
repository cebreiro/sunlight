#include "item_mix_change_step_message_handler.h"

#include "sl/generator/game/contents/group/game_group.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/item_trade_message_creator.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/player_group_system.h"

namespace sunlight
{
    void ItemMixChangeStepMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        (void)reader;

        system.Get<ItemArchiveSystem>().TryRollbackMixItem(player);

        player.Send(ItemTradeMessageCreator::CreateGoldChangeResult(group.GetId()));
    }
}

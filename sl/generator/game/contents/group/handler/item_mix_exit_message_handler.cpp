#include "item_mix_exit_message_handler.h"

#include "sl/generator/game/system/player_group_system.h"

namespace sunlight
{
    void ItemMixExitMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        (void)reader;

        system.ProcessHostExit(group, player);
    }
}

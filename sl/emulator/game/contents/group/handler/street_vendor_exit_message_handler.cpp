#include "street_vendor_exit_message_handler.h"

#include "sl/emulator/game/contents/group/game_group.h"
#include "sl/emulator/game/system/player_group_system.h"

namespace sunlight
{
    void StreetVendorExitMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        (void)reader;

        if (group.IsHost(player))
        {
            system.ProcessHostExit(group, player);
        }
        else
        {
            system.ProcessGuestExit(group, player);
        }
    }
}

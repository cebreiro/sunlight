#include "server_command_item_gain.h"

#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandItemGain::ServerCommandItemGain(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandItemGain::GetName() const -> std::string_view
    {
        return "item_gain";
    }

    bool ServerCommandItemGain::Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const
    {
        int32_t addQuantity = 0;

        return _system.Get<ItemArchiveSystem>().GainItem(player, itemId, quantity, addQuantity);
    }
}

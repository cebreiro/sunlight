#include "server_command_item_add.h"

#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandItemAdd::ServerCommandItemAdd(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandItemAdd::GetName() const -> std::string_view
    {
        return "item_add";
    }

    bool ServerCommandItemAdd::Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const
    {
        return _system.Get<ItemArchiveSystem>().AddItem(player, itemId, quantity);
    }
}

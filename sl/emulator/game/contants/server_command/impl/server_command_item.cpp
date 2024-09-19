#include "server_command_item.h"

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

    auto ServerCommandItemAdd::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandItemAdd::Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const
    {
        return _system.Get<ItemArchiveSystem>().AddItem(player, itemId, quantity);
    }

    ServerCommandItemGain::ServerCommandItemGain(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandItemGain::GetName() const -> std::string_view
    {
        return "item_gain";
    }

    auto ServerCommandItemGain::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandItemGain::Execute(GamePlayer& player, int32_t itemId, int32_t quantity) const
    {
        int32_t addQuantity = 0;

        return _system.Get<ItemArchiveSystem>().GainItem(player, itemId, quantity, addQuantity);
    }
}

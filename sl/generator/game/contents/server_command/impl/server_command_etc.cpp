#include "server_command_etc.h"

#include "sl/generator/game/system/entity_damage_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandSuicide::ServerCommandSuicide(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandSuicide::GetName() const -> std::string_view
    {
        return "suicide";
    }

    auto ServerCommandSuicide::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandSuicide::Execute(GamePlayer& player) const
    {
        _system.Get<EntityDamageSystem>().KillPlayer(player);

        return true;
    }

    ServerCommandGoldAdd::ServerCommandGoldAdd(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandGoldAdd::GetName() const -> std::string_view
    {
        return "gold_add";
    }

    auto ServerCommandGoldAdd::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandGoldAdd::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<ItemArchiveSystem>().AddGold(player, value);

        return true;
    }
}

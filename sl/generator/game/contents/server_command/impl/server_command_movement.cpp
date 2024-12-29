#include "server_command_movement.h"

#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandTeleport::ServerCommandTeleport(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandTeleport::GetName() const -> std::string_view
    {
        return "teleport";
    }

    auto ServerCommandTeleport::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandTeleport::Execute(GamePlayer& player, int32_t x, int32_t y) const
    {
        Eigen::Vector2f position;
        position.x() = static_cast<float>(x);
        position.y() = static_cast<float>(y);

        _system.Get<EntityMovementSystem>().Teleport(player, position);

        return true;
    }
}

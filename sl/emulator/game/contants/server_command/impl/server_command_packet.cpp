#include "server_command_packet.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/server/packet/creator/zone_packet_s2c_creator.h"

namespace sunlight
{
    auto ServerCommandPacketObjectLeave::GetName() const -> std::string_view
    {
        return "packet_object_leave";
    }

    auto ServerCommandPacketObjectLeave::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandPacketObjectLeave::Execute(GamePlayer& player, int32_t id) const
    {
        player.Send(ZonePacketS2CCreator::CreateObjectLeave(id));

        return true;
    }
}

#include "server_command_packet.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/game_player_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/server_command_system.h"
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

    ServerCommandPacketHairColorChange::ServerCommandPacketHairColorChange(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandPacketHairColorChange::GetName() const -> std::string_view
    {
        return "packet_hair_color_change";
    }

    auto ServerCommandPacketHairColorChange::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandPacketHairColorChange::Execute(GamePlayer& player, int32_t color) const
    {
        _system.Get<EntityViewRangeSystem>().Broadcast(player,
            GamePlayerMessageCreator::CreatePlayerHairColorChange(player, color), true);

        return true;
    }

    ServerCommandPacketSkinColorChange::ServerCommandPacketSkinColorChange(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandPacketSkinColorChange::GetName() const -> std::string_view
    {
        return "packet_skin_color_change";
    }

    auto ServerCommandPacketSkinColorChange::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandPacketSkinColorChange::Execute(GamePlayer& player, int32_t color) const
    {
        _system.Get<EntityViewRangeSystem>().Broadcast(player,
            GamePlayerMessageCreator::CreatePlayerSkinColorChange(player, color), true);

        return true;
    }
}

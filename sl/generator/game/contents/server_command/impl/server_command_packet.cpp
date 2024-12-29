#include "server_command_packet.h"

#include "sl/generator/game/entity/game_entity_network_id.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/creator/game_player_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"

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

    auto ServerCommandPacketTest::GetName() const -> std::string_view
    {
        return "packet_test";
    }

    auto ServerCommandPacketTest::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandPacketTest::Execute(GamePlayer& player) const
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write<int32_t>(0x3F2);

        player.Send(writer.Flush());

        return true;
    }
}

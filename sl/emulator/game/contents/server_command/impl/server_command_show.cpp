#include "server_command_show.h"

#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto ServerCommandShowStringTable::GetName() const -> std::string_view
    {
        return "show_string_table";
    }

    auto ServerCommandShowStringTable::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandShowStringTable::Execute(GamePlayer& player, int32_t index) const
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::STRING_TABLE_DEBUG_SHOW);
        writer.Write<int32_t>(index);

        player.Send(writer.Flush());

        return true;
    }
}

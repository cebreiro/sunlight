#include "chat_message_creator.h"

#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto ChatMessageCreator::CreateNormalChat(const GamePlayer& player, const std::string& message) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::CHAT_STRING);
        writer.WriteString(message);

        return writer.Flush();
    }

    auto ChatMessageCreator::CreateShoutChat(const std::string& sender, const std::string& message) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_SHOUT_MESSAGE);
        writer.WriteString(sender);
        writer.WriteString(message);
        writer.Write<int32_t>(0);

        return writer.Flush();
    }

    auto ChatMessageCreator::CreateTradeChat(const std::string& sender, const std::string& message) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_SHOUT_MESSAGE);
        writer.WriteString(sender);
        writer.WriteString(message);
        writer.Write<int32_t>(1);

        return writer.Flush();
    }

    auto ChatMessageCreator::CreateEchoChat(const std::string& sender, const std::string& message) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_SHOUT_MESSAGE);
        writer.WriteString(sender);
        writer.WriteString(message);
        writer.Write<int32_t>(2);

        return writer.Flush();
    }

    auto ChatMessageCreator::CreateServerMessage(const std::string& message) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::SERVER_MESSAGE);
        writer.WriteString(message);

        return writer.Flush();
    }

    auto ChatMessageCreator::CreateServerNotice(const std::string& message) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NOTICE_MESSAGE);
        writer.WriteString(message);

        return writer.Flush();
    }
}

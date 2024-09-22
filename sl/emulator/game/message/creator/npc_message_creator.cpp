#include "npc_message_creator.h"

#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/entity/game_npc.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto NPCMessageCreator::CreateChatting(const GameNPC& npc, const std::string& str) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::CHAT_STRING);
        writer.WriteString(str);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxCreate(const GameNPC& npc, const GamePlayer& player, int32_t width, int32_t height) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_CREATE);
        writer.Write<int32_t>(width);
        writer.Write<int32_t>(height);

        // client 0x49B8F8
        // to find local player and send 'npc direction', 'script state'
        writer.Write<int32_t>(static_cast<int32_t>(player.GetType()));
        writer.Write<int32_t>(static_cast<int32_t>(player.GetId().Unwrap()));

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxClose(const GameNPC& npc) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::CHARSTATUSMSG);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxClose(game_entity_id_type id) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(id, GameEntityType::NPC).ToBuffer());
        writer.Write(ZoneMessageType::CHARSTATUSMSG);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxClear(const GameNPC& npc) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_CLEAR_CONTENTS);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddMenu(const GameNPC& npc, int32_t base, int32_t mouseOver, int32_t index) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDMENU);
        writer.Write<int32_t>(base);
        writer.Write<int32_t>(mouseOver);
        writer.Write<int32_t>(index);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddString(const GameNPC& npc, int32_t tableIndex) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDSTRING);
        writer.Write<int32_t>(tableIndex);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddRuntimeIntString(const GameNPC& npc, int32_t tableIndex, int32_t runtimeValue) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDRTVALUE);
        writer.Write<int32_t>(tableIndex);
        writer.Write<int32_t>(runtimeValue);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddItemName(const GameNPC& npc, int32_t tableIndex, int32_t itemId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDITEMNAME);
        writer.Write<int32_t>(tableIndex);
        writer.Write<int32_t>(itemId);

        return writer.Flush();
    }
}

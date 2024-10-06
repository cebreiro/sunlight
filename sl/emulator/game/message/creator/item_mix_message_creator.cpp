#include "item_mix_message_creator.h"

#include "sl/emulator/game/contants/group/game_group_type.h"
#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"
#include "sl/emulator/service/gamedata/item/item_data.h"

namespace sunlight
{
    auto ItemMixMessageCreator::CreateItemMixWindowByPropSuccess(const GamePlayer& player, int32_t itemId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_OPEN_ITEM_MIX_WINDOW_BY_PROP);
        writer.Write<int32_t>(itemId);

        return writer.Flush();
    }

    auto ItemMixMessageCreator::CreateItemMixWindowByPropFail(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_OPEN_ITEM_MIX_WINDOW_BY_PROP);
        writer.Write<int32_t>(0);

        return writer.Flush();
    }

    auto ItemMixMessageCreator::CreateItemMixWindowByItemSuccess(const GamePlayer& player, const GameItem& item) -> Buffer
    {
        // internal client bug 0x43E8C3
        // if you use item archive msg, then client discards item's entity type
        // (client receives 8-byte id and type, but in the process of copying, discards 4-byte type and store '0')
        // it causes client crash when sending item mix start request to server because of failure of finding item object
        // so use SLV2_OPEN_ITEM_MIX_WINDOW_BY_PROP instead of SLV2_OPEN_ITEM_MIX_WINDOW_BY_ITEM
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_OPEN_ITEM_MIX_WINDOW_BY_PROP);
        writer.Write<int32_t>(item.GetData().GetId());

        return writer.Flush();
    }

    auto ItemMixMessageCreator::CreateItemMixWindowByItemFail(const GamePlayer& player, ItemMixOpenByItemFailReason result) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::SLV2_OPEN_ITEM_MIX_WINDOW_BY_ITEM);

        {
            PacketWriter objectWriter;
            objectWriter.Write(result);
            objectWriter.Write(0);

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto ItemMixMessageCreator::CreateGroupCreate(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::RESPONSE_CREATE_GROUP);
        writer.Write<int32_t>(groupId);
        writer.Write(GameGroupType::ItemMix);

        return writer.Flush();
    }

    auto ItemMixMessageCreator::CreateItemLowerResult(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1148);

        return writer.Flush();
    }

    auto ItemMixMessageCreator::CreateItemMixSuccess(int32_t groupId, int32_t itemId, int32_t mixSkillLevel, int32_t mixSkillExp) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1199);
        writer.Write<int32_t>(0); // 1: start client timer(1s). when it expired, send '1147' to server. 
        writer.Write<int32_t>(itemId);
        writer.Write<int32_t>(mixSkillExp);
        writer.Write<int32_t>(mixSkillLevel);

        return writer.Flush();
    }

    auto ItemMixMessageCreator::CreateItemMixFailure(int32_t groupId, int32_t mixSkillLevel, int32_t mixSkillExp) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1199);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(mixSkillExp);
        writer.Write<int32_t>(mixSkillLevel);

        return writer.Flush();
    }
}

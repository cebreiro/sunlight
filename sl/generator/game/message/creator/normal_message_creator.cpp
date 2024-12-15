#include "normal_message_creator.h"

#include "sl/generator/game/contents/group/game_group_type.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto NormalMessageCreator::CreateChangeRoom(int32_t destStageId, int32_t destX, int32_t destY) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::CHANGE_ROOM);
        writer.Write<int32_t>(destX);
        writer.Write<int32_t>(destY);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(destStageId);

        return writer.Flush();
    }

    auto NormalMessageCreator::CreateItemStreetVendorCreateFail(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::RESPONSE_CREATE_GROUP_FAILED);
        writer.Write<int32_t>(groupId);
        writer.Write(GameGroupType::StreetVendor);
        writer.Write<int8_t>(0);

        return writer.Flush();
    }
}

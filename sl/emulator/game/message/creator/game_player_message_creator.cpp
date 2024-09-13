#include "game_player_message_creator.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/creator/zone_data_transfer_object_creator.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto GamePlayerMessageCreator::CreateAllState(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteInt64(static_cast<int32_t>(player.GetId().Unwrap()), static_cast<int32_t>(player.GetType()));
        writer.Write(ZoneMessageType::CONTAIN_ALL_STATE);
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerUnkState(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerAppearance(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerInformation(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerPet(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerSkill(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerStatusEffect(player));

        return writer.Flush();
    }
}

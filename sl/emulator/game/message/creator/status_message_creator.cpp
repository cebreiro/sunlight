#include "status_message_creator.h"

#include "sl/emulator/game/contents/status_effect/status_effect.h"
#include "sl/emulator/game/entity/game_entity.h"
#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto StatusMessageCreator::CreateStatusEffectAdd(const GameEntity& entity, const StatusEffect& statusEffect) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::CHARSTATUSMSG);
        writer.Write(ZoneMessageType::CONDITION_STATE);

        {
            // client 0x4B37F0
            PacketWriter objectWriter;
            /* a3 + 00 */objectWriter.Write<int32_t>(static_cast<int32_t>(entity.GetId().Unwrap()));
            /* a3 + 04 */objectWriter.Write<int32_t>(static_cast<int32_t>(entity.GetType()));
            /* a3 + 08 */objectWriter.Write<int32_t>(statusEffect.IsHidden() ? 1 : 0);
            /* a3 + 12 */objectWriter.Write<int32_t>(0);

            // client 0x4B443F
            /* a3 + 16 */objectWriter.Write<int32_t>(statusEffect.GetSkillId()); // CONDITION_ACTION.sox -> index
            /* a3 + 20 */objectWriter.Write<int32_t>(1700); // client 0x4787E0. param 1700 works
            /* a3 + 24 */objectWriter.Write<int32_t>(static_cast<int32_t>(statusEffect.GetType()));
            /* a3 + 28 */objectWriter.Write<int32_t>(0);
            /* a3 + 32 */objectWriter.Write<int32_t>(0);
            /* a3 + 36 */objectWriter.Write<int32_t>(statusEffect.GetId());
            /* a3 + 40 */objectWriter.Write<int32_t>(statusEffect.GetStatValue());
            /* a3 + 44 */objectWriter.Write<int32_t>(statusEffect.GetStatType());
            /* a3 + 48 */objectWriter.Write<float>(static_cast<float>(statusEffect.GetStatPercentageValue()) / 100.f);
            /* a3 + 52 */objectWriter.Write<int32_t>(statusEffect.GetType() == StatusEffectType::DyingResistance ? 777 : 0); // client 0x4B49F0

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto StatusMessageCreator::CreateStatusEffectRemove(const GameEntity& entity, const StatusEffect& statusEffect) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::CHARSTATUSMSG);
        writer.Write(ZoneMessageType::CONDITION_END);

        {
            PacketWriter objectWriter;
            /* a3 + 00 */objectWriter.Write<int32_t>(static_cast<int32_t>(entity.GetId().Unwrap()));
            /* a3 + 04 */objectWriter.Write<int32_t>(static_cast<int32_t>(entity.GetType()));
            /* a3 + 08 */objectWriter.Write<int32_t>(statusEffect.IsHidden() ? 1 : 0);
            /* a3 + 12 */objectWriter.Write<int32_t>(0);

            // client 0x4B443F
            /* a3 + 16 */objectWriter.Write<int32_t>(statusEffect.GetSkillId()); // CONDITION_ACTION.sox -> index
            /* a3 + 20 */objectWriter.Write<int32_t>(0);
            /* a3 + 24 */objectWriter.Write<int32_t>(static_cast<int32_t>(statusEffect.GetType()));
            /* a3 + 28 */objectWriter.Write<int32_t>(0);
            /* a3 + 32 */objectWriter.Write<int32_t>(0);
            /* a3 + 36 */objectWriter.Write<int32_t>(statusEffect.GetId());
            /* a3 + 40 */objectWriter.Write<int32_t>(0);
            /* a3 + 44 */objectWriter.Write<int32_t>(statusEffect.GetStatType());
            /* a3 + 48 */objectWriter.Write<float>(0);
            /* a3 + 52 */objectWriter.Write<int32_t>(0);

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }
}

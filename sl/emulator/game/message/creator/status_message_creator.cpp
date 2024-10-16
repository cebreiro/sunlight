#include "status_message_creator.h"

#include "sl/emulator/game/contents/status_effect/status_effect.h"
#include "sl/emulator/game/entity/game_entity.h"
#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"
#include "sl/emulator/service/gamedata/skill/skill_effect_data.h"

namespace sunlight
{
    auto StatusMessageCreator::CreateStatusEffectAdd(const GameEntity& entity, const StatusEffect& effect) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::CHARSTATUSMSG);
        writer.Write(ZoneMessageType::CONDITION_STATE);

        {
            PacketWriter objectWriter;
            /* a3 + 00 */objectWriter.Write<int32_t>(static_cast<int32_t>(entity.GetId().Unwrap()));
            /* a3 + 04 */objectWriter.Write<int32_t>(static_cast<int32_t>(entity.GetType()));
            /* a3 + 08 */objectWriter.Write<int32_t>(effect.IsHidden() ? 1 : 0);
            /* a3 + 12 */objectWriter.Write<int32_t>(0);

            // client 0x4B443F
            /* a3 + 16 */objectWriter.Write<int32_t>(effect.GetSkillId()); // CONDITION_ACTION.sox -> index
            /* a3 + 20 */objectWriter.Write<int32_t>(0);
            /* a3 + 24 */objectWriter.Write<int32_t>(static_cast<int32_t>(effect.GetType()));
            /* a3 + 28 */objectWriter.Write<int32_t>(0);
            /* a3 + 32 */objectWriter.Write<int32_t>(0);
            /* a3 + 36 */objectWriter.Write<int32_t>(effect.GetId());
            /* a3 + 40 */objectWriter.Write<int32_t>(effect.GetSkillEffectData().value8); // stat value
            /* a3 + 44 */objectWriter.Write<int32_t>(effect.GetSkillEffectData().value7); // stat type
            /* a3 + 48 */objectWriter.Write<float>(static_cast<float>(effect.GetSkillEffectData().value9) / 100.f); // stat percentage value
            /* a3 + 52 */objectWriter.Write<int32_t>(effect.GetType() == StatusEffectType::DyingResistance ? 777 : 0); // client 0x4B49F0

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }
}

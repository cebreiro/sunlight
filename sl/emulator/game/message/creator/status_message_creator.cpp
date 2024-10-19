#include "status_message_creator.h"

#include "sl/emulator/game/contents/attack/attack_result.h"
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

    auto StatusMessageCreator::CreateAttackResult(const GameEntity& entity, const AttackResult& attackResult) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::DAMAGE);

        {
            // client 0x48EB80
            PacketWriter objectWriter;
            /* a3 + 00 */objectWriter.Write<int32_t>(static_cast<int32_t>(attackResult.attackerId.Unwrap()));// + 2
            /* a3 + 04 */objectWriter.Write<int32_t>(static_cast<int32_t>(attackResult.attackerType)); // + 3
            /* a3 + 08 */objectWriter.Write<int32_t>(attackResult.id);
            /* a3 + 12 */objectWriter.Write<int32_t>(attackResult.motionId); // 3
            /* a3 + 16 */objectWriter.Write<int32_t>(0);

            /* a3 + 20, a2[12] */objectWriter.Write<int32_t>(static_cast<int32_t>(attackResult.attackTargetBlowType));
            /* a3 + 24, a2[11] */objectWriter.Write<int32_t>(attackResult.skillId);
            /* a3 + 28,        */objectWriter.Write<int32_t>(attackResult.damage);
            /* a3 + 32, a2[10] */objectWriter.Write<int32_t>(static_cast<int32_t>(attackResult.weaponClass)); // weapon class
            /* a3 + 36         */objectWriter.Write<int32_t>(static_cast<int32_t>(attackResult.damageType));
            /* a3 + 40         */objectWriter.Write<int32_t>(attackResult.damageCount);
            /* a3 + 44, a2[5]  */objectWriter.Write<int32_t>(attackResult.damageInterval);
            /* a3 + 48, a2[14] */objectWriter.Write<int32_t>(attackResult.attackBlowGroup);
            /* a3 + 52*/objectWriter.Write<int32_t>(static_cast<int32_t>(attackResult.attackedResultType));

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }
}

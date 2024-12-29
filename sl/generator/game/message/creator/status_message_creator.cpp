#include "status_message_creator.h"

#include "sl/generator/game/contents/damage/damage_result.h"
#include "sl/generator/game/contents/status_effect/status_effect.h"
#include "sl/generator/game/entity/game_entity.h"
#include "sl/generator/game/entity/game_entity_network_id.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/creator/zone_data_transfer_object_creator.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"

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
        writer.WriteObject(ZoneDataTransferObjectCreator::CreateStatusEffect(entity, statusEffect));

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
        writer.WriteObject(ZoneDataTransferObjectCreator::CreateStatusEffect(entity, statusEffect));

        return writer.Flush();
    }

    auto StatusMessageCreator::CreateDamageResult(const GameEntity& entity, const DamageResult& damageResult) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::DAMAGE);

        {
            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(static_cast<int32_t>(damageResult.attackerId.Unwrap()));
            objectWriter.Write<int32_t>(static_cast<int32_t>(damageResult.attackerType));
            objectWriter.Write<int32_t>(damageResult.id);
            objectWriter.Write<int32_t>(damageResult.motionId);
            objectWriter.Write<int32_t>(0);

            objectWriter.Write<int32_t>(static_cast<int32_t>(damageResult.blowType));
            objectWriter.Write<int32_t>(damageResult.skillId);
            objectWriter.Write<int32_t>(damageResult.damage);
            objectWriter.Write<int32_t>(static_cast<int32_t>(damageResult.weaponClass)); // weapon class
            objectWriter.Write<int32_t>(static_cast<int32_t>(damageResult.damageType));
            objectWriter.Write<int32_t>(damageResult.damageCount);
            objectWriter.Write<int32_t>(damageResult.damageInterval);
            objectWriter.Write<int32_t>(damageResult.blowGroup);
            objectWriter.Write<int32_t>(static_cast<int32_t>(damageResult.attackedResultType));

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto StatusMessageCreator::CreateHPChange(const GameEntity& entity, int32_t maxHP, int32_t hp, HPChangeFloaterType floater) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::CONTAIN_HP);
        writer.Write<int32_t>(maxHP);
        writer.Write<int32_t>(hp);
        writer.Write(floater);

        return writer.Flush();
    }

    auto StatusMessageCreator::CreateSPChange(const GameEntity& entity, int32_t maxSP, int32_t sp, SPChangeFloaterType floater) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::CONTAIN_SP);
        writer.Write<int32_t>(maxSP);
        writer.Write<int32_t>(sp);
        writer.Write(floater);

        return writer.Flush();
    }
}

#include "scene_object_message_creator.h"

#include <boost/container/static_vector.hpp>

#include "sl/generator/game/component/entity_state_component.h"
#include "sl/generator/game/component/entity_status_effect_component.h"
#include "sl/generator/game/component/item_ownership_component.h"
#include "sl/generator/game/component/monster_stat_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/entity/game_entity_network_id.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/entity/game_stored_item.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/creator/zone_data_transfer_object_creator.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"
#include "sl/generator/service/gamedata/item/item_data.h"

namespace sunlight
{
    auto SceneObjectPacketCreator::CreateState(const GameEntity& entity) -> Buffer
    {
        assert(entity.HasComponent<EntityStateComponent>());

        return CreateState(entity, entity.GetComponent<EntityStateComponent>().GetState());
    }

    auto SceneObjectPacketCreator::CreateState(const GameEntity& entity, const GameEntityState& state) -> Buffer
    {
        assert(entity.HasComponent<EntityStateComponent>());

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(entity).ToBuffer());
        writer.Write(ZoneMessageType::CHAR_STATE);

        {
            PacketWriter objectWriter;
            objectWriter.Write<uint8_t>(state.bitmask1);
            objectWriter.Write<uint8_t>(state.bitmask2);
            objectWriter.Write<int8_t>(static_cast<int8_t>(state.type));
            objectWriter.Write<int8_t>(static_cast<int8_t>(state.moveType));
            objectWriter.Write<uint8_t>(state.unk4);
            objectWriter.Write<uint8_t>(state.unk5);
            objectWriter.Write<float>(state.destPosition.x());
            objectWriter.Write<float>(state.destPosition.y());
            objectWriter.Write<float>(state.destPosition.z());

            objectWriter.Write<uint32_t>(state.targetId.Unwrap());
            objectWriter.Write<uint32_t>(static_cast<uint32_t>(state.targetType));
            objectWriter.Write<int32_t>(state.attackId);
            objectWriter.Write<int32_t>(state.motionId);
            objectWriter.Write<int32_t>(state.fxId);
            objectWriter.Write<int32_t>(state.param1);
            objectWriter.Write<int32_t>(state.param2);
            objectWriter.Write<int32_t>(state.skillId);

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateInformation(const GamePlayer& player, bool showSpawnEffect) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_USERINFO);
        writer.Write<int32_t>(player.GetComponent<SceneObjectComponent>().GetId());
        writer.Write(static_cast<int32_t>(player.GetType()));
        writer.Write(static_cast<int32_t>(player.GetId().Unwrap()));
        writer.Write<int32_t>(player.GetStatComponent().GetGender());
        writer.WriteString(player.GetName());
        writer.Write<int8_t>(player.IsArmed() ? 1 : 0);
        writer.Write<int8_t>(showSpawnEffect ? 0 : 1);

        writer.Write<int32_t>(player.IsDead() ? static_cast<int32_t>(GameEntityStateType::Dead) : 0);

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateInformation(const GameNPC& npc) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_USERINFO);
        writer.Write<int32_t>(npc.GetComponent<SceneObjectComponent>().GetId());
        writer.Write(static_cast<int32_t>(npc.GetType()));
        writer.Write(static_cast<int32_t>(npc.GetId().Unwrap()));
        writer.Write<int32_t>(npc.GetPNX());
        writer.WriteString(npc.GetName());
        writer.Write<int32_t>(0); // unk

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateInformation(const GameItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_USERINFO);
        writer.Write<int32_t>(item.GetComponent<SceneObjectComponent>().GetId());
        writer.Write(static_cast<int32_t>(item.GetType()));
        writer.Write(static_cast<int32_t>(item.GetId().Unwrap()));
        writer.Write(item.GetData().GetModelId());

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateInformation(const GameStoredItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_USERINFO);
        writer.Write<int32_t>(item.GetComponent<SceneObjectComponent>().GetId());
        writer.Write(static_cast<int32_t>(item.GetType()));
        writer.Write(static_cast<int32_t>(item.GetId().Unwrap()));
        writer.Write(item.GetData().GetModelId());
        writer.Write(item.GetGroupId());

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateInformation(const GameMonster& monster, bool showSpawnEffect) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_USERINFO);
        writer.Write<int32_t>(monster.GetComponent<SceneObjectComponent>().GetId());
        writer.Write(static_cast<int32_t>(monster.GetType()));
        writer.Write(static_cast<int32_t>(monster.GetId().Unwrap()));
        writer.Write<int8_t>(monster.IsInvisible() ? 1 : 0);
        writer.Write<int32_t>(monster.GetDataId());
        writer.Write<int32_t>(monster.GetStatComponent().GetHP().As<int32_t>());
        writer.Write<int8_t>(showSpawnEffect ? 0 : 1);

        const EntityStatusEffectComponent& statusEffectComponent = monster.GetStatusEffectComponent();

        const int32_t buffCount = static_cast<int32_t>(statusEffectComponent.GetCount());
        writer.Write<int32_t>(buffCount);

        if (buffCount > 0)
        {
            for (const StatusEffect& statusEffect : statusEffectComponent.GetRange())
            {
                writer.WriteObject(ZoneDataTransferObjectCreator::CreateStatusEffect(monster, statusEffect));
            }
        }

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateItemDisplay(const GameItem& item, int64_t characterId) -> Buffer
    {
        const ItemOwnershipComponent* ownershipComponent = item.FindComponent<ItemOwnershipComponent>();

        const int32_t remainOwnershipDuration = ownershipComponent ? ownershipComponent->GetRemainDurationMilli(GameTimeService::Now()) : 0;
        const bool displayOwnership = remainOwnershipDuration > 0;

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(item).ToBuffer());
        writer.Write(ZoneMessageType::CREATEITEM);
        writer.Write<int8_t>(displayOwnership ? 1 : 0);
        writer.Write<int8_t>(displayOwnership && ownershipComponent->IsOwner(characterId) ? 1 : 0);

        // -1 : new item (spawn animation)
        //  0 : none
        // other : client crash
        writer.Write<int32_t>(0);

        const SceneObjectComponent& sceneObjectComponent = item.GetComponent<SceneObjectComponent>();
        const auto& pos = sceneObjectComponent.GetPosition();

        writer.Write<int32_t>(static_cast<int32_t>(pos.x()));
        writer.Write<int32_t>(static_cast<int32_t>(pos.y()));
        writer.Write<int32_t>(static_cast<int32_t>(pos.x()));
        writer.Write<int32_t>(static_cast<int32_t>(pos.y()));
        writer.Write<int16_t>(static_cast<int16_t>(sceneObjectComponent.GetYaw()));

        writer.Write<int32_t>(remainOwnershipDuration);

        {
            boost::container::static_vector<char, 23> buffer;
            StreamWriter objectWriter(buffer);

            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(static_cast<uint16_t>(item.GetQuantity()));
            objectWriter.Write<int32_t>(item.GetData().GetId()); // 65
            objectWriter.Write<int8_t>(0);
            objectWriter.Write<int32_t>(item.GetId().Unwrap());
            objectWriter.Write<int32_t>(static_cast<uint32_t>(item.GetType()));

            writer.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
            writer.WriteObject(buffer);
        }

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateItemSpawn(const GameItem& item, int64_t characterId, Eigen::Vector2f originPos) -> Buffer
    {
        const ItemOwnershipComponent* ownershipComponent = item.FindComponent<ItemOwnershipComponent>();

        const int32_t remainOwnershipDuration = ownershipComponent ? ownershipComponent->GetRemainDurationMilli(GameTimeService::Now()) : 0;
        const bool displayOwnership = remainOwnershipDuration > 0;

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(item).ToBuffer());
        writer.Write(ZoneMessageType::CREATEITEM);
        writer.Write<int8_t>(displayOwnership ? 1 : 0);
        writer.Write<int8_t>(displayOwnership && ownershipComponent->IsOwner(characterId) ? 1 : 0);

        // -1 : new item (spawn animation)
        //  0 : none
        // other : client crash
        writer.Write<int32_t>(-1);

        const SceneObjectComponent& sceneObjectComponent = item.GetComponent<SceneObjectComponent>();
        const auto& pos = sceneObjectComponent.GetPosition();

        writer.Write<int32_t>(static_cast<int32_t>(originPos.x()));
        writer.Write<int32_t>(static_cast<int32_t>(originPos.y()));
        writer.Write<int32_t>(static_cast<int32_t>(pos.x()));
        writer.Write<int32_t>(static_cast<int32_t>(pos.y()));
        writer.Write<int16_t>(static_cast<int16_t>(sceneObjectComponent.GetYaw()));

        writer.Write<int32_t>(remainOwnershipDuration);

        {
            boost::container::static_vector<char, 23> buffer;
            StreamWriter objectWriter(buffer);

            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(static_cast<uint16_t>(item.GetQuantity()));
            objectWriter.Write<int32_t>(item.GetData().GetId()); // 65
            objectWriter.Write<int8_t>(0);
            objectWriter.Write<int32_t>(item.GetId().Unwrap());
            objectWriter.Write<int32_t>(static_cast<uint32_t>(item.GetType()));

            writer.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
            writer.WriteObject(buffer);
        }

        return writer.Flush();
    }

    auto SceneObjectPacketCreator::CreateItemDisplay(const GameStoredItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(item).ToBuffer());
        writer.Write(ZoneMessageType::CREATEITEM);
        writer.Write<int32_t>(static_cast<int32_t>(item.GetOwnerType()));
        writer.Write<int32_t>(static_cast<int32_t>(item.GetOwnerId().Unwrap()));
        writer.Write<int32_t>(item.GetPage());
        writer.Write<int32_t>(item.GetPrice());

        const SceneObjectComponent& sceneObjectComponent = item.GetComponent<SceneObjectComponent>();
        const auto& pos = sceneObjectComponent.GetPosition();
        const float yaw = sceneObjectComponent.GetYaw();
        const float radian = static_cast<float>(yaw * std::numbers::pi / 180.0);

        Eigen::Vector2f direction(std::cos(radian), std::sin(radian));
        direction.normalize();
        
        writer.Write<float>(pos.y());
        writer.Write<float>(pos.x());
        writer.Write<float>(direction.y());
        writer.Write<float>(direction.x());

        {
            boost::container::static_vector<char, 23> buffer;
            StreamWriter objectWriter(buffer);

            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(static_cast<uint16_t>(item.GetQuantity()));
            objectWriter.Write<int32_t>(item.GetData().GetId());
            objectWriter.Write<int8_t>(0);
            objectWriter.Write<int32_t>(item.GetId().Unwrap());
            objectWriter.Write<int32_t>(static_cast<uint32_t>(item.GetType()));

            writer.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
            writer.WriteObject(buffer);
        }

        return writer.Flush();
    }
}

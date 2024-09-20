#include "zone_packet_s2c_creator.h"

#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"

namespace sunlight
{
    auto ZonePacketS2CCreator::CreateLoginAccept(const GamePlayer& player, int32_t stageId) -> Buffer
    {
        const SceneObjectComponent& sceneObjectComponent = player.GetSceneObjectComponent();

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_LOGIN_ACCEPT);

        {
            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(sceneObjectComponent.GetId());
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<float>(sceneObjectComponent.GetPosition().x());
            objectWriter.Write<float>(sceneObjectComponent.GetPosition().y());
            objectWriter.Write<float>(sceneObjectComponent.GetYaw());
            objectWriter.Write<float>(sceneObjectComponent.GetSpeed());
            objectWriter.Write<float>(sceneObjectComponent.GetUnk1());
            objectWriter.Write<float>(sceneObjectComponent.GetDestPosition().x());
            objectWriter.Write<float>(sceneObjectComponent.GetDestPosition().y());
            objectWriter.Write<uint16_t>(sceneObjectComponent.GetUnk2());
            objectWriter.Write<uint16_t>(sceneObjectComponent.GetUnk3());

            writer.WriteObject(objectWriter);
        }

        writer.WriteString(std::to_string(player.GetZoneId()));
        writer.Write(player.GetId().Unwrap());
        writer.Write(player.GetType());
        writer.Write<int32_t>(player.GetStatComponent().GetGender());
        writer.Write<int32_t>(stageId);

        // maybe time
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.WriteZeroBytes(4);

        return writer.Flush();
    }

    auto ZonePacketS2CCreator::CreateLoginReject(const std::string& reason) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_LOGIN_REJECT);
        writer.Write<int8_t>(0);
        writer.WriteString(reason);

        return writer.Flush();
    }

    auto ZonePacketS2CCreator::CreateObjectLeave(const GameEntity& entity) -> Buffer
    {
        assert(entity.FindComponent<SceneObjectComponent>());

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_OBJECT_LEAVE);
        writer.Write<int32_t>(entity.GetComponent<SceneObjectComponent>().GetId());

        return writer.Flush();
    }

    auto ZonePacketS2CCreator::CreateObjectLeave(int32_t id) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_OBJECT_LEAVE);
        writer.Write<int32_t>(id);

        return writer.Flush();
    }

    auto ZonePacketS2CCreator::CreateObjectMove(const GameEntity& entity) -> Buffer
    {
        assert(entity.FindComponent<SceneObjectComponent>());

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_OBJECT_MOVE);
        {
            const auto& sceneObjectComponent = entity.GetComponent<SceneObjectComponent>();

            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(sceneObjectComponent.GetId());
            objectWriter.Write<float>(sceneObjectComponent.GetPosition().x());
            objectWriter.Write<float>(sceneObjectComponent.GetPosition().y());
            objectWriter.Write<float>(sceneObjectComponent.GetYaw());
            objectWriter.Write<float>(sceneObjectComponent.GetSpeed());
            objectWriter.Write<float>(sceneObjectComponent.GetUnk1());
            objectWriter.Write<float>(sceneObjectComponent.GetDestPosition().x());
            objectWriter.Write<float>(sceneObjectComponent.GetDestPosition().y());
            objectWriter.Write<uint16_t>(sceneObjectComponent.GetUnk2());
            objectWriter.Write<uint16_t>(sceneObjectComponent.GetUnk3());

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto ZonePacketS2CCreator::CreateObjectForceMove(const GameEntity& entity) -> Buffer
    {
        assert(entity.FindComponent<SceneObjectComponent>());

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_FORCE_MOVE);
        {
            const auto& sceneObjectComponent = entity.GetComponent<SceneObjectComponent>();

            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(sceneObjectComponent.GetId());
            objectWriter.Write<float>(sceneObjectComponent.GetPosition().x());
            objectWriter.Write<float>(sceneObjectComponent.GetPosition().y());
            objectWriter.Write<float>(sceneObjectComponent.GetYaw());
            objectWriter.Write<float>(sceneObjectComponent.GetSpeed());
            objectWriter.Write<float>(sceneObjectComponent.GetUnk1());
            objectWriter.Write<float>(sceneObjectComponent.GetDestPosition().x());
            objectWriter.Write<float>(sceneObjectComponent.GetDestPosition().y());
            objectWriter.Write<uint16_t>(sceneObjectComponent.GetUnk2());
            objectWriter.Write<uint16_t>(sceneObjectComponent.GetUnk3());

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto ZonePacketS2CCreator::CreateObjectRoomChange(int32_t roomId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_ROOM_CHANGE);
        writer.Write<int32_t>(roomId);

        return writer.Flush();
    }

    auto ZonePacketS2CCreator::CreateObjectVisibleRange(float range) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_VISIBLE_RANGE);
        writer.Write<float>(range);

        return writer.Flush();
    }
}

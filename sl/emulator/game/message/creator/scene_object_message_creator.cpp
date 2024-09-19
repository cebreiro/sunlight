#include "scene_object_message_creator.h"

#include <boost/container/static_vector.hpp>

#include "sl/emulator/game/component/item_ownership_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"
#include "sl/emulator/service/gamedata/item/item_data.h"

namespace sunlight
{
    auto SceneObjectPacketCreator::CreateInformation(const GameItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_USERINFO); // handled on client 4E6A40
        writer.Write<int32_t>(item.GetComponent<SceneObjectComponent>().GetId());
        writer.Write(static_cast<int32_t>(item.GetType()));
        writer.Write(static_cast<int32_t>(item.GetId().Unwrap()));
        writer.Write(item.GetData().GetModelId());

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

            // client 0x459BE0
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

            // client 0x459BE0
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
}

#include "item_archive_message_creator.h"

#include "sl/emulator/game/component/item_position_component.h"
#include "sl/emulator/game/component/player_item_component.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"
#include "sl/emulator/service/gamedata/item/item_data.h"

namespace sunlight
{
    auto ItemArchiveMessageCreator::CreateInit(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_INIT);

        const PlayerItemComponent& itemComponent = player.GetItemComponent();;

        const auto itemRange = itemComponent.GetItemRange();
        const int64_t itemCount = std::ssize(itemRange);

        writer.Write<int32_t>(static_cast<int32_t>(itemCount));
        writer.Write<int32_t>(itemComponent.GetInventoryPage());
        writer.Write<int32_t>(itemComponent.GetGold());

        // picked item
        GameEntityNetworkId pickedItemId = itemComponent.GetPickedItem() ?
            GameEntityNetworkId(*itemComponent.GetPickedItem()) : GameEntityNetworkId::Null();

        writer.WriteObject(pickedItemId.ToBuffer());

        // equip_item
        {
            const auto& equipItems = itemComponent.
                GetEquipItems();

            PacketWriter objectWriter;

            for (int64_t i = 0; i < std::ssize(equipItems); ++i)
            {
                if (const GameItem* equipItem = equipItems[i]; equipItem)
                {
                    objectWriter.Write<uint32_t>(equipItem->GetId().Unwrap());
                    objectWriter.Write<uint32_t>(static_cast<uint32_t>(equipItem->GetType()));
                }
                else
                {
                    objectWriter.Write<int64_t>(0);
                }
            }

            writer.WriteObject(objectWriter);
        }

        // all items
        {
            PacketWriter objectWriter;

            for (const GameItem& item : itemRange)
            {
                const ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

                constexpr int32_t byteSize = 23;
                objectWriter.Write<int32_t>(byteSize);

                // ~ 0
                objectWriter.Write<int16_t>(positionComponent.IsInQuickSlot());
                objectWriter.Write<int16_t>(positionComponent.GetPage());
                objectWriter.Write<int16_t>(positionComponent.GetX());
                objectWriter.Write<int16_t>(positionComponent.GetY());
                objectWriter.Write<uint16_t>(static_cast<uint16_t>(item.GetQuantity()));
                objectWriter.Write<int32_t>(item.GetData().GetId());
                objectWriter.Write<int8_t>(0); // unk
                objectWriter.Write<uint32_t>(item.GetId().Unwrap());
                objectWriter.Write<uint32_t>(static_cast<uint32_t>(item.GetType()));
                // ~ byteSize
            }

            const int64_t objectSize = objectWriter.GetWriteSize();

            writer.Write<int32_t>(static_cast<int32_t>(objectSize));
            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateArchiveResult(const GamePlayer& player, bool result, ZoneMessageType archiveMessage) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_RESULT);
        writer.Write<int32_t>(result);
        writer.Write(archiveMessage);

        return writer.Flush();
    }
}

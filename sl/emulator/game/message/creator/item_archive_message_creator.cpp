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

        const PlayerItemComponent& itemComponent = player.GetItemComponent();

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

    auto ItemArchiveMessageCreator::CreateNewPickedItemAdd(const GamePlayer& player, const GameItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_ADDITEM);

        const item_object_buffer_type buffer = CreateItemObject(item);
        writer.Write<int32_t>(static_cast<int32_t>(buffer.size()));
        writer.WriteObject(buffer);

        writer.Write<int32_t>(1);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemLift(const GamePlayer& player, const GameItem& pickedItem) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_LIFTITEM);

        writer.WriteObject(GameEntityNetworkId(pickedItem).ToBuffer());
        writer.WriteObject(GameEntityNetworkId(pickedItem).ToBuffer());
        writer.Write<int32_t>(-1);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemLift(const GamePlayer& player, const GameItem& pickedItem,
        const GameItem& origin, int32_t decreaseQuantity) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_LIFTITEM);

        writer.WriteObject(GameEntityNetworkId(pickedItem).ToBuffer());
        writer.WriteObject(GameEntityNetworkId(origin).ToBuffer());
        writer.Write<int32_t>(decreaseQuantity);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateInventoryItemAdd(const GamePlayer& player, const GameItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_ADDINVENITEM);

        const item_object_buffer_type buffer = CreateItemObject(item);
        writer.Write<int32_t>(static_cast<int32_t>(buffer.size()));
        writer.WriteObject(buffer);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemAdd(const GamePlayer& player, const GameItem& item, int32_t quantity) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_ADDITEM);

        {
            const ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

            PacketWriter objectWriter;

            // client 0x459BE0
            objectWriter.Write<int16_t>(positionComponent.IsInQuickSlot());
            objectWriter.Write<int16_t>(positionComponent.GetPage());
            objectWriter.Write<int16_t>(positionComponent.GetX());
            objectWriter.Write<int16_t>(positionComponent.GetY());
            objectWriter.Write<int16_t>(static_cast<int16_t>(quantity));
            objectWriter.Write<int32_t>(item.GetData().GetId());
            objectWriter.Write<int8_t>(0); // unk
            objectWriter.Write<int32_t>(item.GetId().Unwrap());
            objectWriter.Write<int32_t>(static_cast<uint32_t>(item.GetType()));

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        // client 45560B
        // 0: inventory/quick_slot item quantity, 1: create new pick item
        // client 450CEF
        // if itemEdibleData.bUseQuickSlot is true, client priorly treats page, x, y as quick_slot position(if not empty)
        // so, before create bUseQuickSlot item on inventory, must priorly fill all quick slots.
        writer.Write<int32_t>(0);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemDecrease(const GamePlayer& player, const GameItem& item, int32_t quantity) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_DECREASEITEM);
        writer.WriteObject(GameEntityNetworkId(item).ToBuffer());
        writer.Write<int32_t>(quantity);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemRemove(const GamePlayer& player, game_entity_id_type removed, GameEntityType removedType) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_REMOVEITEM);
        writer.WriteObject(GameEntityNetworkId(removed, removedType).ToBuffer());

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

    auto ItemArchiveMessageCreator::CreateItemObject(const GameItem& item) -> item_object_buffer_type
    {
        item_object_buffer_type result = {};

        const ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

        StreamWriter writer(result);

        // client 0x459BE0
        writer.Write<int16_t>(positionComponent.IsInQuickSlot());
        writer.Write<int16_t>(positionComponent.GetPage());
        writer.Write<int16_t>(positionComponent.GetX());
        writer.Write<int16_t>(positionComponent.GetY());
        writer.Write<int16_t>(static_cast<uint16_t>(item.GetQuantity())); // 63
        writer.Write<int32_t>(item.GetData().GetId()); // 65
        writer.Write<int8_t>(0); // 69
        writer.Write<int32_t>(item.GetId().Unwrap()); // 70
        writer.Write<int32_t>(static_cast<uint32_t>(item.GetType())); // 74

        return result;
    }
}

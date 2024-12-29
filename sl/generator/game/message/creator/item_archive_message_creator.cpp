#include "item_archive_message_creator.h"

#include "sl/generator/game/component/item_position_component.h"
#include "sl/generator/game/component/player_account_storage_component.h"
#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/entity/game_entity_network_id.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"
#include "sl/generator/service/gamedata/item/item_data.h"

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

    auto ItemArchiveMessageCreator::CreateItemAddForRefresh(const GamePlayer& player, int32_t itemId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_ADDITEM);

        {
            PacketWriter objectWriter;

            objectWriter.Write<int16_t>(0);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(-1);
            objectWriter.Write<int16_t>(1);
            objectWriter.Write<int32_t>(itemId);
            objectWriter.Write<int8_t>(0); // unk
            objectWriter.Write<int32_t>(GameConstant::ITEM_ENTITY_ID_FOR_INVENTORY_REFRESH);
            objectWriter.Write<int32_t>(static_cast<uint32_t>(GameEntityType::Item));

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        writer.Write<int32_t>(0);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemRemoveForRefresh(const GamePlayer& player) -> Buffer
    {
        return CreateItemRemove(player, game_entity_id_type(GameConstant::ITEM_ENTITY_ID_FOR_INVENTORY_REFRESH), GameEntityType::Item);
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

    auto ItemArchiveMessageCreator::CreateItemDecrease(const GamePlayer& player, game_entity_id_type target,
        GameEntityType targetType, int32_t quantity) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_DECREASEITEM);
        writer.WriteObject(GameEntityNetworkId(target, targetType).ToBuffer());
        writer.Write<int32_t>(quantity);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemRemove(const GamePlayer& player, game_entity_id_type target, GameEntityType targetType) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_REMOVEITEM);
        writer.WriteObject(GameEntityNetworkId(target, targetType).ToBuffer());

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateGoldAddOrSub(const GamePlayer& player, int32_t value) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_ADD_SUB_GOLD);
        writer.Write<int32_t>(value);

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

    auto ItemArchiveMessageCreator::CreateAccountStorageOpening(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::PLAYER_OPEN_STORAGE);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateAccountStorageInit(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ACCOUNTSTORAGE_ARCHIVEMSG);
        writer.Write(ZoneMessageType::ACCOUNTSTORAGE_ARCHIVE_INIT);

        const PlayerAccountStorageComponent* accountStorageComponent = player.FindComponent<PlayerAccountStorageComponent>();
        if (!accountStorageComponent)
        {
            assert(false);

            return writer.Flush();
        }

        writer.Write<int32_t>(accountStorageComponent->GetPage());
        writer.Write<int32_t>(accountStorageComponent->GetGold());

        const auto itemRange = accountStorageComponent->GetItemRange();
        writer.Write<int32_t>(static_cast<int32_t>(std::ssize(itemRange)));

        {
            PacketWriter objectWriter;

            for (const GameItem& item : itemRange)
            {
                const ItemPositionComponent& itemPositionComponent = item.GetComponent<ItemPositionComponent>();

                objectWriter.Write<int32_t>(itemPositionComponent.GetPage());
                objectWriter.Write<int32_t>(itemPositionComponent.GetX());
                objectWriter.Write<int32_t>(itemPositionComponent.GetY());

                const item_object_buffer_type buffer = CreateItemObject(item);

                objectWriter.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
                objectWriter.WriteBuffer(buffer);
            }

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateAccountStorageUnlock(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ACCOUNTSTORAGE_ARCHIVEMSG);
        writer.Write(ZoneMessageType::ACCOUNTSTORAGE_ARCHIVE_RESULT);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateAccountStorageGoldAddOrSub(const GamePlayer& player, int32_t value) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::ACCOUNTSTORAGE_ARCHIVEMSG);
        writer.Write(ZoneMessageType::ACCOUNTSTORAGE_ARCHIVE_ADD_SUB_GOLD);
        writer.Write<int32_t>(value);

        return writer.Flush();
    }

    auto ItemArchiveMessageCreator::CreateItemObject(const GameItem& item) -> item_object_buffer_type
    {
        item_object_buffer_type result = {};

        const ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

        StreamWriter writer(result);

        writer.Write<int16_t>(positionComponent.IsInQuickSlot());
        writer.Write<int16_t>(positionComponent.GetPage());
        writer.Write<int16_t>(positionComponent.GetX());
        writer.Write<int16_t>(positionComponent.GetY());
        writer.Write<int16_t>(static_cast<uint16_t>(item.GetQuantity()));
        writer.Write<int32_t>(item.GetData().GetId());
        writer.Write<int8_t>(2);
        writer.Write<int32_t>(item.GetId().Unwrap());
        writer.Write<int32_t>(static_cast<uint32_t>(item.GetType()));

        return result;
    }
}

#include "npc_message_creator.h"

#include "sl/generator/game/component/item_position_component.h"
#include "sl/generator/game/component/npc_item_shop_component.h"
#include "sl/generator/game/entity/game_entity_network_id.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/creator/item_archive_message_creator.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"
#include "sl/generator/service/gamedata/item/item_data.h"

namespace sunlight
{
    auto NPCMessageCreator::CreateChatting(const GameNPC& npc, const std::string& str) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::CHAT_STRING);
        writer.WriteString(str);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxCreate(const GameNPC& npc, const GamePlayer& player, int32_t width, int32_t height) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_CREATE);
        writer.Write<int32_t>(width);
        writer.Write<int32_t>(height);

        writer.Write<int32_t>(static_cast<int32_t>(player.GetType()));
        writer.Write<int32_t>(static_cast<int32_t>(player.GetId().Unwrap()));

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxClose(const GameNPC& npc) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::CHARSTATUSMSG);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxClose(game_entity_id_type id) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(id, GameEntityType::NPC).ToBuffer());
        writer.Write(ZoneMessageType::CHARSTATUSMSG);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxClear(const GameNPC& npc) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_CLEAR_CONTENTS);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddMenu(const GameNPC& npc, int32_t base, int32_t mouseOver, int32_t index) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDMENU);
        writer.Write<int32_t>(base);
        writer.Write<int32_t>(mouseOver);
        writer.Write<int32_t>(index);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddString(const GameNPC& npc, int32_t tableIndex) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDSTRING);
        writer.Write<int32_t>(tableIndex);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddRuntimeIntString(const GameNPC& npc, int32_t tableIndex, int32_t runtimeValue) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDRTVALUE);
        writer.Write<int32_t>(tableIndex);
        writer.Write<int32_t>(runtimeValue);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateTalkBoxAddItemName(const GameNPC& npc, int32_t tableIndex, int32_t itemId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPCTALKBOX_ADDITEMNAME);
        writer.Write<int32_t>(tableIndex);
        writer.Write<int32_t>(itemId);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateShopOpen(const GameNPC& npc) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::NPC_OPEN_SHOPWINDOW);

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateItemSynchroStart(const GameNPC& npc) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::NPCITEMARCHIVE_STARTSYNCHRO);

        const NPCItemShopComponent* itemShopComponent = npc.FindComponent<NPCItemShopComponent>();
        if (itemShopComponent)
        {
            PacketWriter objectWriter;

            objectWriter.Write<int32_t>(static_cast<int32_t>(itemShopComponent->GetItemCount()));

            for (const GameItem& item : itemShopComponent->GetItemRange())
            {
                const boost::container::static_vector<char, 23>& itemObject = ItemArchiveMessageCreator::CreateItemObject(item);

                objectWriter.Write<int32_t>(static_cast<int32_t>(std::ssize(itemObject)));
                objectWriter.WriteBuffer(itemObject);
            }

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }
        else
        {
            PacketWriter objectWriter;

            objectWriter.Write<int32_t>(0);

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateNPCItemAdd(const GameNPC& npc, const GameItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_ADDITEM);

        {
            PacketWriter objectWriter;
            objectWriter.WriteBuffer(ItemArchiveMessageCreator::CreateItemObject(item));

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateNPCItemRemove(const GameNPC& npc, game_entity_id_type targetId, GameEntityType targetType) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_REMOVEITEM);

        writer.WriteObject(GameEntityNetworkId(targetId, targetType).ToBuffer());

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateNPCItemDecrease(const GameNPC& npc, const GameItem& item, int32_t newQuantity) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::ITEMARCHIVE_DECREASEITEM);

        writer.WriteObject(GameEntityNetworkId(item).ToBuffer());
        writer.Write<int32_t>(0);

        {
            const ItemPositionComponent& positionComponent = item.GetComponent<ItemPositionComponent>();

            PacketWriter objectWriter;

            objectWriter.Write<int16_t>(positionComponent.IsInQuickSlot());
            objectWriter.Write<int16_t>(positionComponent.GetPage());
            objectWriter.Write<int16_t>(positionComponent.GetX());
            objectWriter.Write<int16_t>(positionComponent.GetY());
            objectWriter.Write<int16_t>(static_cast<int16_t>(newQuantity));
            objectWriter.Write<int32_t>(item.GetData().GetId());
            objectWriter.Write<int8_t>(static_cast<int8_t>(0xAA)); // unk
            objectWriter.Write<int32_t>(item.GetId().Unwrap());
            objectWriter.Write<int32_t>(static_cast<uint32_t>(item.GetType()));

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto NPCMessageCreator::CreateNPCItemArchiveResult(const GameNPC& npc, NPCItemShopResult result) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(npc).ToBuffer());
        writer.Write(ZoneMessageType::ITEMARCHIVEMSG);
        writer.Write(ZoneMessageType::NPCITEMARCHIVE_RESULT);
        writer.Write(result);

        return writer.Flush();
    }
}

#include "item_trade_message_creator.h"

#include "sl/generator/game/component/player_item_trade_component.h"
#include "sl/generator/game/contents/group/game_group_type.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/creator/item_archive_message_creator.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto ItemTradeMessageCreator::CreateGroupCreate(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::RESPONSE_CREATE_GROUP);
        writer.Write<int32_t>(groupId);
        writer.Write(GameGroupType::Trade);

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateGroupHostData(int32_t groupId, const GamePlayer& host) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(200);

        {
            PacketWriter objectWriter;
            objectWriter.WriteZeroBytes(156); // group data

            writer.WriteObject(objectWriter);
        }

        writer.Write<int32_t>(1); // guest count
        writer.Write<int32_t>(static_cast<int32_t>(host.GetType()));
        writer.Write<int32_t>(static_cast<int32_t>(host.GetId().Unwrap()));

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateGroupHostItemData(int32_t groupId, const GamePlayer& host) -> Buffer
    {
        assert(host.HasComponent<PlayerItemTradeComponent>());

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1008);

        const PlayerItemTradeComponent& itemTradeComponent = host.GetComponent<PlayerItemTradeComponent>();

        writer.Write<int32_t>(itemTradeComponent.GetGold());
        writer.Write<int32_t>(static_cast<int32_t>(std::ssize(itemTradeComponent.GetItems())));

        for (const auto& pair : itemTradeComponent.GetItems() | std::views::values)
        {
            {
                const boost::container::static_vector<char, 23> buffer = ItemArchiveMessageCreator::CreateItemObject(*pair.first);

                writer.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
                writer.WriteObject(buffer);
                writer.Write<int32_t>(pair.second.x);
                writer.Write<int32_t>(pair.second.y);
            }
        }

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateGroupGuestData(int32_t groupId, const GamePlayer& guest) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(700);
        writer.Write<int32_t>(static_cast<int32_t>(guest.GetType()));
        writer.Write<int32_t>(static_cast<int32_t>(guest.GetId().Unwrap()));

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateGroupJoinFail(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(300);

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateGroupGuestExit(int32_t groupId, const GamePlayer& host) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(800);
        writer.Write<int32_t>(static_cast<int32_t>(host.GetType()));
        writer.Write<int32_t>(static_cast<int32_t>(host.GetId().Unwrap()));

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateGoldChange(int32_t groupId, int32_t gold) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1003);
        writer.Write<int32_t>(gold);

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateGoldChangeResult(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1155);

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateLiftItem(int32_t groupId, game_entity_id_type itemId, GameEntityType itemType) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1001);
        writer.WriteInt64(static_cast<int32_t>(itemId.Unwrap()), static_cast<int32_t>(itemType));

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateLowerItem(int32_t groupId, int32_t x, int32_t y, const GameItem& item) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1002);
        writer.Write<int32_t>(x);
        writer.Write<int32_t>(y);

        {
            const boost::container::static_vector<char, 23> buffer = ItemArchiveMessageCreator::CreateItemObject(item);

            writer.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
            writer.WriteObject(buffer);
        }

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateLiftItemResult(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1010); // 1001 - 1010

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateLowerItemResult(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1011); // 1002 - 1011

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateTradeConfirm(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1005);

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateTradeSuccess(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1052);

        return writer.Flush();
    }

    auto ItemTradeMessageCreator::CreateTradeFail(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1051);

        return writer.Flush();
    }
}

#include "street_vendor_message_creator.h"

#include "sl/generator/game/component/player_item_component.h"
#include "sl/generator/game/component/street_vendor_host_component.h"
#include "sl/generator/game/contents/group/game_group_type.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/creator/item_archive_message_creator.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"

namespace sunlight
{
    auto StreetVendorMessageCreator::CreateGroupCreate(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::RESPONSE_CREATE_GROUP);
        writer.Write<int32_t>(groupId);
        writer.Write(GameGroupType::StreetVendor);
        writer.Write<int32_t>(GameConstant::MAX_STREET_VENDOR_PAGE_SIZE);

        return writer.Flush();
    }

    auto StreetVendorMessageCreator::CreateGroupCreationFail(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::RESPONSE_CREATE_GROUP_FAILED);
        writer.Write<int32_t>(groupId);
        writer.Write(GameGroupType::StreetVendor);
        writer.Write<int32_t>(0);

        return writer.Flush();
    }

    auto StreetVendorMessageCreator::CreateGroupShutdown(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1307);

        return writer.Flush();
    }

    auto StreetVendorMessageCreator::CreatePageItemDisplay(int32_t groupId, int32_t page, const GamePlayer& player) -> Buffer
    {
        assert(player.HasComponent<StreetVendorHostComponent>());

        const PlayerItemComponent& itemComponent = player.GetItemComponent();
        const GameItem* first = itemComponent.GetVendorSaleItem(page);
        const GameItem* second = itemComponent.GetVendorSaleItem(page + 1);

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1301);
        writer.Write<int32_t>(0); // 0: set item, 1: set price
        writer.Write<int32_t>(GameConstant::MAX_STREET_VENDOR_PAGE_SIZE);
        writer.Write<int32_t>(page);

        if (first)
        {
            writer.Write<int32_t>(1);
            writer.Write<int32_t>(player.GetComponent<StreetVendorHostComponent>().GetItemPrice(page));

            boost::container::static_vector<char, 23> buffer = ItemArchiveMessageCreator::CreateItemObject(*first);

            writer.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
            writer.WriteObject(buffer);
        }
        else
        {
            writer.Write<int32_t>(0);
        }

        if (second)
        {
            writer.Write<int32_t>(1);
            writer.Write<int32_t>(player.GetComponent<StreetVendorHostComponent>().GetItemPrice(page + 1));

            boost::container::static_vector<char, 23> buffer = ItemArchiveMessageCreator::CreateItemObject(*second);

            writer.Write<int32_t>(static_cast<int32_t>(std::ssize(buffer)));
            writer.WriteObject(buffer);
        }
        else
        {
            writer.Write<int32_t>(0);
        }

        return writer.Flush();
    }

    auto StreetVendorMessageCreator::CreateItemPriceChange(int32_t groupId, int32_t page, int32_t price) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1301);
        writer.Write<int32_t>(0); // 0: set item, 1: set price
        writer.Write<int32_t>(GameConstant::MAX_STREET_VENDOR_PAGE_SIZE);
        writer.Write<int32_t>(page);
        writer.Write<int32_t>(price);

        return writer.Flush();
    }

    auto StreetVendorMessageCreator::CreateOpenResult(int32_t groupId, StreetVendorStartResult result) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1306);
        writer.Write(result);

        return writer.Flush();
    }

    auto StreetVendorMessageCreator::CreateItemPurchaseResult(int32_t groupId, StreetVendorPurchaseResult result) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1308);
        writer.Write(result);

        return writer.Flush();
    }

    auto StreetVendorMessageCreator::CreateVendorItemSoldOut(int32_t groupId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::GROUP_MSG);
        writer.Write<int32_t>(groupId);
        writer.Write<int32_t>(900);
        writer.Write<int32_t>(1309);

        return writer.Flush();
    }
}

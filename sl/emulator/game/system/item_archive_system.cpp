#include "item_archive_system.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/message/creator/item_archive_message_creator.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    ItemArchiveSystem::ItemArchiveSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void ItemArchiveSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool ItemArchiveSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::ITEMARCHIVEMSG,
            std::bind_front(&ItemArchiveSystem::HandleMessage, this)))
        {
            return false;
        }

        return true;
    }

    auto ItemArchiveSystem::GetName() const -> std::string_view
    {
        return "item_archive_system";
    }

    auto ItemArchiveSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<ItemArchiveSystem>();
    }

    void ItemArchiveSystem::HandleMessage(const ZoneMessage& message) const
    {
        GamePlayer& player = message.player;
        SlPacketReader& reader = message.reader;
        game_entity_id_type targetId = message.targetId;
        GameEntityType targetType = message.targetType;

        switch (const auto subType = reader.Read<ZoneMessageType>(); subType)
        {
        case ZoneMessageType::ITEMARCHIVE_INIT:
        {
            player.Send(ItemArchiveMessageCreator::CreateInit(player));
        }
        break;
        case ZoneMessageType::ITEMARCHIVE_ADDITEM:
        case ZoneMessageType::ITEMARCHIVE_LIFTITEM:
        case ZoneMessageType::ITEMARCHIVE_LOWERITEM:
        case ZoneMessageType::ITEMARCHIVE_LOWERITEM_QI:
        case ZoneMessageType::ITEMARCHIVE_LOWERWEAR:
        case ZoneMessageType::ITEMARCHIVE_DROPITEM:
        case ZoneMessageType::ITEMARCHIVE_USEITEM:
        case ZoneMessageType::ITEMARCHIVE_ADD_SUB_GOLD:
        case ZoneMessageType::ITEMARCHIVE_REMOVEITEM:
        case ZoneMessageType::ITEMARCHIVE_RESULT:
        case ZoneMessageType::ITEMARCHIVE_LORDITEM:
        case ZoneMessageType::ITEMARCHIVE_ALLINVEN:
        case ZoneMessageType::ITEMARCHIVE_DROPMONEY:
        case ZoneMessageType::ITEMARCHIVE_DECREASEITEM:
        case ZoneMessageType::ITEMARCHIVE_ADDINVENITEM:
        case ZoneMessageType::ITEMARCHIVE_REALIGNITEM:
        case ZoneMessageType::ITEMARCHIVE_LOCAL_USE_ITEM:
        default:
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] unhandled zone message. player: {}, type: {}, target: [{}, {}]",
                    GetName(), player.GetCId(), ToString(subType), targetId, ToString(targetType)));

        }
    }
}

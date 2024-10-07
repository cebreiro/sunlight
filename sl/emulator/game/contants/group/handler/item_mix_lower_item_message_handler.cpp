#include "item_mix_lower_item_message_handler.h"

#include "sl/emulator/game/contants/group/game_group.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/item_mix_message_creator.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/player_group_system.h"
#include "sl/emulator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void ItemMixLowerItemMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        [[maybe_unused]]
        const int32_t unk1 = reader.Read<int32_t>();

        const auto [itemId, itemType] = reader.ReadInt64();

        if (system.Get<ItemArchiveSystem>().OnMixItemLower(player, game_entity_id_type(itemId)))
        {
            player.Send(ItemMixMessageCreator::CreateItemLowerResult(group.GetId()));
        }
        else
        {
            SUNLIGHT_LOG_ERROR(system.GetServiceLocator(),
                fmt::format("fail to lower mix item. player: {}, item_id: {}",
                    player.GetCId(), itemId));
        }
    }
}

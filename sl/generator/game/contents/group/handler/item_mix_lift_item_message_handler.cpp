#include "item_mix_lift_item_message_handler.h"

#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/player_group_system.h"
#include "sl/generator/server/packet/io/sl_packet_reader.h"

namespace sunlight
{
    void ItemMixLiftItemMessageHandler::Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const
    {
        (void)group;

        [[maybe_unused]]
        const int32_t unk1 = reader.Read<int32_t>();
        const auto [itemId, itemType] = reader.ReadInt64();

        if (!system.Get<ItemArchiveSystem>().OnMixItemLift(player, game_entity_id_type(itemId)))
        {
            SUNLIGHT_LOG_ERROR(system.GetServiceLocator(),
                fmt::format("fail to lift mix item. player: {}, item_id: {}",
                    player.GetCId(), itemId));
        }
    }
}

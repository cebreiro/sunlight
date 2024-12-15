#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"
#include "sl/generator/game/message/zone_message_type.h"

namespace sunlight
{
    class GamePlayer;
    class SlPacketReader;
}

namespace sunlight
{
    struct ZoneMessage
    {
        GamePlayer& player;
        ZoneMessageType type = {};
        game_entity_id_type targetId = {};
        GameEntityType targetType = {};
        SlPacketReader& reader;
    };
}

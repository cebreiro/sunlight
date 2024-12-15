#pragma once
#include "sl/generator/game/message/character_message_type.h"

namespace sunlight
{
    class GamePlayer;
    class SlPacketReader;
}

namespace sunlight
{
    struct CharacterMessage
    {
        GamePlayer& player;
        std::string targetName;
        CharacterMessageType type = {};
        SlPacketReader& reader;
    };
}

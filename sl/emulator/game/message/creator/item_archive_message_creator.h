#pragma once
#include "sl/emulator/game/message/zone_message_type.h"

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class ItemArchiveMessageCreator
    {
    public:
        ItemArchiveMessageCreator() = delete;

        static auto CreateInit(const GamePlayer& player) -> Buffer;

        static auto CreateArchiveResult(const GamePlayer& player, bool result, ZoneMessageType archiveMessage) -> Buffer;
    };
}

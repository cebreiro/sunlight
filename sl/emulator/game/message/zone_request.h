#pragma once
#include "sl/emulator/server/packet/zone_packet_c2s.h"

namespace sunlight
{
    class GamePlayer;
    class SlPacketReader;
}

namespace sunlight
{
    struct ZoneRequest
    {
        GamePlayer& player;
        ZonePacketC2S type;
        SlPacketReader& reader;
    };
}

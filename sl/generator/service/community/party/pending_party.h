#pragma once
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    struct PendingParty
    {
        int64_t id = 0;
        int64_t leaderId = 0;
        std::string partyName;

        std::string invited;
    };
}

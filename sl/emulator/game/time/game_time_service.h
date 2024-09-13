#pragma once
#include "sl/emulator/game/time/game_time.h"

namespace sunlight
{
    class GameTimeService
    {
    public:
        GameTimeService() = delete;

        static auto Get() -> game_time_point_type;

        static void Set(game_time_point_type time);

    private:
        static thread_local game_time_point_type _timePoint;
    };
}

#pragma once
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    class GameTimeService
    {
    public:
        GameTimeService() = delete;

        static auto Now() -> game_time_point_type;

        static void SetNow(game_time_point_type time);

    private:
        static thread_local game_time_point_type _timePoint;
    };
}

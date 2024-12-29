#include "game_time_service.h"

namespace sunlight
{
    thread_local game_time_point_type GameTimeService::_timePoint;

    auto GameTimeService::Now() -> game_time_point_type
    {
        return _timePoint;
    }

    void GameTimeService::SetNow(game_time_point_type timePoint)
    {
        _timePoint = timePoint;
    }
}

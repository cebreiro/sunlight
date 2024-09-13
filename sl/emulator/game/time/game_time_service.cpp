#include "game_time_service.h"

namespace sunlight
{
    thread_local game_time_point_type GameTimeService::_timePoint;

    auto GameTimeService::Get() -> game_time_point_type
    {
        return _timePoint;
    }

    void GameTimeService::Set(game_time_point_type timePoint)
    {
        _timePoint = timePoint;
    }
}

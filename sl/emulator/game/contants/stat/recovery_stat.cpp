#include "recovery_stat.h"

namespace sunlight
{
    void RecoveryStat::Update(game_time_point_type timePoint)
    {
        assert(timePoint >= _lastUpdateTimePoint);

        if (_lastUpdateTimePoint == timePoint)
        {
            return;
        }

        _lastUpdateTimePoint = timePoint;

        const double count = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(timePoint - _lastUpdateTimePoint).count());

        _value =  std::clamp(_value + _regenValue * count, _minValue, _maxValue);
    }

    void RecoveryStat::ChangeRegenValue(game_time_point_type timePoint, StatValue value)
    {
        Update(timePoint);

        _regenValue = value;
    }

    auto RecoveryStat::GetValue(game_time_point_type timePoint) const -> StatValue
    {
        const_cast<RecoveryStat*>(this)->Update(timePoint);

        return _value;
    }

    auto RecoveryStat::GetMaxValue() const -> StatValue
    {
        return _maxValue;
    }

    auto RecoveryStat::GetLastUpdateTimePoint() const -> game_time_point_type
    {
        return _lastUpdateTimePoint;
    }

    void RecoveryStat::SetValue(StatValue value)
    {
        _value = value;
    }

    void RecoveryStat::SetRegenValue(StatValue value)
    {
        _regenValue = value;
    }

    void RecoveryStat::SetMinValue(StatValue value)
    {
        _minValue = value;
    }

    void RecoveryStat::SetMaxValue(StatValue value)
    {
        _maxValue = value;
    }
}

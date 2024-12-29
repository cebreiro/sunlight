#include "recovery_stat.h"

namespace sunlight
{
    bool RecoveryStat::IsDisabled() const
    {
        return _disabled;
    }

    void RecoveryStat::Update(game_time_point_type timePoint)
    {
        assert(timePoint >= _lastUpdateTimePoint);

        if (_disabled)
        {
            return;
        }

        if (_lastUpdateTimePoint == timePoint)
        {
            return;
        }

        const double tickCount = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(timePoint - _lastUpdateTimePoint).count());
        const double count = tickCount / _regenTickCount;

        _value =  std::clamp(_value + _regenValue * count, _minValue, _maxValue);
        _lastUpdateTimePoint = timePoint;
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

    auto RecoveryStat::GetRegenStateFactor() const -> StatValue
    {
        return _regenStateFactor;
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
        _value = std::clamp(value, _minValue, _maxValue);
    }

    void RecoveryStat::SetRegenValue(StatValue value)
    {
        _regenValue = value;
    }

    void RecoveryStat::SetRegenStateFactor(StatValue value)
    {
        _regenStateFactor = value;
    }

    void RecoveryStat::SetMinValue(StatValue value)
    {
        _minValue = value;
    }

    void RecoveryStat::SetMaxValue(StatValue value)
    {
        _maxValue = value;
    }

    void RecoveryStat::SetRegenTickCount(int32_t value)
    {
        _regenTickCount = value;
    }

    void RecoveryStat::SetUpdateTimePoint(game_time_point_type timePoint)
    {
        _lastUpdateTimePoint = timePoint;
    }

    void RecoveryStat::SetDisable(bool value)
    {
        _disabled = value;
    }
}

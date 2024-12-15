#pragma once
#include "sl/generator/game/contents/stat/stat_value.h"
#include "sl/generator/game/time/game_time.h"

namespace sunlight
{
    ENUM_CLASS(RecoveryStatType, int32_t,
        (HP)
        (SP)

        (Count)
    )

    class RecoveryStat
    {
    public:
        RecoveryStat() = default;

        bool IsDisabled() const;

        void Update(game_time_point_type timePoint);
        void ChangeRegenValue(game_time_point_type timePoint, StatValue value);

        auto GetValue(game_time_point_type timePoint) const -> StatValue;
        auto GetRegenStateFactor() const -> StatValue;
        auto GetMaxValue() const -> StatValue;
        auto GetLastUpdateTimePoint() const -> game_time_point_type;

        void SetValue(StatValue value);
        void SetRegenValue(StatValue value);
        void SetRegenStateFactor(StatValue value);
        void SetMinValue(StatValue value);
        void SetMaxValue(StatValue value);
        void SetRegenTickCount(int32_t value);
        void SetUpdateTimePoint(game_time_point_type timePoint);
        void SetDisable(bool value);

    private:
        StatValue _value = {};
        StatValue _minValue = {};
        StatValue _maxValue = {};
        StatValue _regenValue = {};
        StatValue _regenStateFactor = {};
        int32_t _regenTickCount = 1;

        game_time_point_type _lastUpdateTimePoint = game_clock_type::now();
        bool _disabled = true;
    };
}

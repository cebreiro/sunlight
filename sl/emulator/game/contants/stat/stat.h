#pragma once
#include "sl/emulator/game/contants/stat/stat_value.h"

namespace sunlight
{
    ENUM_CLASS(StatOriginType, int32_t,
        (Base, 0)
        (Item)
        (Skill)
        (StatusEffect)
        (StatusEffectPercentage)
        (Count)
    )

    class Stat
    {
    public:
        auto Get(StatOriginType type) const -> StatValue;
        auto GetFinalValue() const -> StatValue;

        void Set(StatOriginType type, StatValue value);

    private:
        void Update();

    private:
        std::array<StatValue, static_cast<int32_t>(StatOriginType::Count)> _values = {};

        bool _dirty = false;
        StatValue _finalValue = {};
    };
}

#include "stat.h"

namespace sunlight
{
    auto Stat::Get(StatOriginType type) const -> StatValue
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_values));

        return _values[index];
    }

    auto Stat::GetFinalValue() const -> StatValue
    {
        if (_dirty)
        {
            const_cast<Stat*>(this)->Update();
        }

        return _finalValue;
    }

    void Stat::Set(StatOriginType type, StatValue value)
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_values));

        _values[index] = value;
    }

    void Stat::Update()
    {
        if (_dirty)
        {
            return;
        }

        _dirty = false;

        const StatValue base = Get(StatOriginType::Base);

        const StatValue sum = base + Get(StatOriginType::Item) + Get(StatOriginType::Skill) + Get(StatOriginType::StatusEffect);
        const StatValue percentage = base + Get(StatOriginType::StatusEffectPercentage);

        _finalValue = sum + percentage;
    }
}

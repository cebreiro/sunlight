#include "stat.h"

namespace sunlight
{
    bool Stat::HasChanges() const
    {
        return _changes;
    }

    auto Stat::Get(StatOriginType type) const -> StatValue
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_values));

        return _values[index];
    }

    auto Stat::GetStatSum() const -> StatValue
    {
        return _statSum;
    }

    auto Stat::GetFinalValue() const -> StatValue
    {
        assert(!_changes);

        return _finalValue;
    }

    void Stat::Set(StatOriginType type, StatValue value)
    {
        const int64_t index = static_cast<int64_t>(type);
        assert(index >= 0 && index < std::ssize(_values));

        _values[index] = value;
    }

    void Stat::SetStatSum(StatValue value)
    {
        _statSum = value;
    }

    void Stat::SetFinalValue(StatValue value)
    {
        _finalValue = value;
    }

    void Stat::SetChanges(bool value)
    {
        _changes = value;
    }
}

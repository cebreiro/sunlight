#include "stat_value.h"

namespace sunlight
{
    StatValue::StatValue(double value)
        : _value(value)
    {
    }

    auto StatValue::operator+(const StatValue& other) const -> StatValue
    {
        return _value + other._value;
    }

    auto StatValue::operator-(const StatValue& other) const -> StatValue
    {
        return _value - other._value;
    }

    auto StatValue::operator*(const StatValue& other) const -> StatValue
    {
        return _value * other._value;
    }

    auto StatValue::operator/(const StatValue& other) const-> StatValue
    {
        const value_type result = _value / other._value;
        assert(!std::isnan(result));

        return result;
    }

    void StatValue::operator+=(const StatValue& other)
    {
        _value += other._value;
    }

    void StatValue::operator-=(const StatValue& other)
    {
        _value -= other._value;
    }

    void StatValue::operator*=(const StatValue& other)
    {
        _value *= other._value;
    }

    void StatValue::operator/=(const StatValue& other)
    {
        _value /= other._value;
        assert(!std::isnan(_value));
    }

    bool StatValue::operator==(const StatValue& other) const
    {
        return std::abs(_value - other._value) <= std::numeric_limits<value_type>::epsilon();
    }

    bool StatValue::operator!=(const StatValue& other) const
    {
        return !this->operator==(other);
    }

    bool StatValue::operator<(const StatValue& other) const
    {
        return _value < other._value;
    }

    bool StatValue::operator<=(const StatValue& other) const
    {
        return _value <= other._value;
    }

    bool StatValue::operator>(const StatValue& other) const
    {
        return _value > other._value;
    }

    bool StatValue::operator>=(const StatValue& other) const
    {
        return _value >= other._value;
    }

    auto StatValue::Zero() -> StatValue
    {
        return StatValue(0.0);
    }
}

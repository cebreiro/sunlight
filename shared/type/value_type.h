#pragma once
#include <concepts>
#include <string>
#include <functional>
#include <fmt/color.h>
#include <boost/container_hash/hash.hpp>

namespace sunlight
{
    template <std::integral T, typename Tag>
    class ValueType
    {
    public:
        using value_type = T;

    public:
        ValueType() = default;
        explicit ValueType(T value);

        explicit operator T() const noexcept;
        auto Unwrap() const noexcept -> T;

        bool operator==(const ValueType& other) const;
        bool operator!=(const ValueType& other) const;
        bool operator<(const ValueType& other) const;

        static auto Default() -> ValueType;

    private:
        T _value = {};
    };

    template <std::integral T, typename Tag>
    ValueType<T, Tag>::ValueType(T value)
        : _value(value)
    {
    }

    template <std::integral T, typename Tag>
    ValueType<T, Tag>::operator T() const noexcept
    {
        return Unwrap();
    }

    template <std::integral T, typename Tag>
    auto ValueType<T, Tag>::Unwrap() const noexcept -> T
    {
        return _value;
    }

    template <std::integral T, typename Tag>
    bool ValueType<T, Tag>::operator==(const ValueType& other) const
    {
        return _value == other._value;
    }

    template <std::integral T, typename Tag>
    bool ValueType<T, Tag>::operator!=(const ValueType& other) const
    {
        return _value != other._value;
    }

    template <std::integral T, typename Tag>
    bool ValueType<T, Tag>::operator<(const ValueType& other) const
    {
        return _value < other._value;
    }

    template <std::integral T, typename Tag>
    auto ValueType<T, Tag>::Default() -> ValueType
    {
        return ValueType(0);
    }

    template <typename T, typename Tag>
    auto hash_value(const ValueType<T, Tag>& item) -> size_t
    {
        boost::hash<T> hasher;
        return hasher(item.Unwrap());
    }
}

namespace std
{
    template <std::integral T, typename Tag>
    struct hash<sunlight::ValueType<T, Tag>>
    {
        size_t operator()(sunlight::ValueType<T, Tag> value) const noexcept {
            return std::hash<T>{}(value.Unwrap());
        }
    };
}

namespace fmt
{
    template <std::integral T, typename Tag>
    struct formatter<sunlight::ValueType<T, Tag>> : formatter<std::string>
    {
        auto format(sunlight::ValueType<T, Tag> value, format_context& context) const
            -> format_context::iterator
        {
            return formatter<std::string>::format(fmt::format("{}", value.Unwrap()), context);
        }
    };
}

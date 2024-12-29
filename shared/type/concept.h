#pragma once
#include <concepts>
#include <optional>

namespace sunlight
{
    template <typename T>
    concept is_optional = requires(T t)
    {
        typename T::value_type;
        requires std::same_as< T, std::optional<typename T::value_type>>;
    };
}

#pragma once

namespace sunlight::detail
{
    class GameEntityIdType
    {
    public:
        using value_type = uint32_t;

    public:
        GameEntityIdType() = default;
        explicit GameEntityIdType(uint32_t value, uint32_t recycleSequence = 0);

        auto Unwrap() const -> uint32_t;
        auto GetRecycleSequence() const -> uint32_t;

        void SetRecycleSequence(uint32_t sequence);

        friend bool operator==(const GameEntityIdType& lhs, const GameEntityIdType& rhs);
        friend bool operator<(const GameEntityIdType& lhs, const GameEntityIdType& rhs);

        friend auto hash_value(GameEntityIdType id) -> size_t;

    private:
        uint32_t _value = 0;
        uint32_t _recycleSequence = 0;
    };
}

namespace sunlight
{
    namespace detail
    {
        class GameEntityIdTypeTag;
    }

    using game_entity_id_type = detail::GameEntityIdType;
}

namespace std
{
    template <>
    struct hash<sunlight::detail::GameEntityIdType>
    {
        size_t operator()(sunlight::detail::GameEntityIdType value) const noexcept {
            return std::hash<uint64_t>{}(value.Unwrap());
        }
    };
}

namespace fmt
{
    template <>
    struct formatter<sunlight::detail::GameEntityIdType> : formatter<std::string>
    {
        auto format(sunlight::detail::GameEntityIdType value, format_context& context) const
            -> format_context::iterator
        {
            return formatter<std::string>::format(fmt::format("{}", value.Unwrap()), context);
        }
    };
}

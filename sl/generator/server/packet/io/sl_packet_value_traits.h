#pragma once

namespace sunlight::detail
{
    template <typename T>
    struct PacketValueTraits;

    struct PrimitiveValueType
    {
        static constexpr bool is_primitive = true;
        static constexpr bool is_object = false;
    };

    struct ObjectValueType
    {
        static constexpr bool is_primitive = false;
        static constexpr bool is_object = true;
    };

    template <>
    struct PacketValueTraits<int8_t> : PrimitiveValueType
    {
        static constexpr uint8_t type_value = 1;
    };

    template <> struct PacketValueTraits<uint8_t> : PacketValueTraits<int8_t> {};

    template <>
    struct PacketValueTraits<int16_t> : PrimitiveValueType
    {
        static constexpr uint8_t type_value = 2;
    };

    template <> struct PacketValueTraits<uint16_t> : PacketValueTraits<int16_t> {};

    template <>
    struct PacketValueTraits<int32_t> : PrimitiveValueType
    {
        static constexpr uint8_t type_value = 3;
    };

    template <> struct PacketValueTraits<uint32_t> : PacketValueTraits<int32_t> {};

    template <>
    struct PacketValueTraits<float> : PrimitiveValueType
    {
        static constexpr uint8_t type_value = 4;
    };

    template <>
    struct PacketValueTraits<std::string> : ObjectValueType
    {
        static constexpr uint8_t type_value = 7;
    };

    struct object_type {};

    template <>
    struct PacketValueTraits<object_type> : ObjectValueType
    {
        static constexpr uint8_t type_value = 0x80;
    };
}

namespace sunlight
{
    template <typename T>
    concept packet_primitive_value_concept = requires
    {
        requires detail::PacketValueTraits<T>::is_primitive;
    };
}

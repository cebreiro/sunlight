#pragma once
#include "sl/emulator/server/packet/io/sl_packet_value_traits.h"

namespace sunlight
{
    class SlPacketReader
    {
    public:
        SlPacketReader(const SlPacketReader& other) = delete;
        SlPacketReader(SlPacketReader&& other) noexcept = delete;
        SlPacketReader& operator=(const SlPacketReader& other) = delete;
        SlPacketReader& operator=(SlPacketReader&& other) noexcept = delete;

    public:
        SlPacketReader() = default;
        explicit SlPacketReader(Buffer buffer);
        ~SlPacketReader();

        template <typename T> requires ((std::integral<T> || std::same_as<T, float>) && sizeof(T) <= 4)
        auto Read() -> T;

        template <typename T> requires ((std::integral<T> || std::same_as<T, float>) && sizeof(T) <= 4)
        auto Peek() const -> T;

        template <typename T> requires std::is_enum_v<T>
        auto Read() -> T;

        template <typename T> requires std::is_enum_v<T>
        auto Peek() const -> T;

        void Skip();

        auto ReadInt64() -> std::pair<int32_t, int32_t>;
        auto ReadUInt64() -> std::pair<uint32_t, uint32_t>;
        auto ReadString() -> std::string;

        auto ReadObject() -> BufferReader;
        void ReadObject(std::vector<char>& result);

        auto GetBuffer() const -> const Buffer&;

    private:
        void Initialize(const Buffer& buffer);

        static void ThrowIfInvalidRead(uint8_t expected, uint8_t result);

    private:
        Buffer _buffer;
        int64_t _index = 0;
        boost::container::small_vector<BufferReader, 16> _readers;
    };

    template <typename T> requires ((std::integral<T> || std::same_as<T, float>) && sizeof(T) <= 4)
    auto SlPacketReader::Read() -> T
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        constexpr uint8_t expected = detail::PacketValueTraits<T>::type_value;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        return reader.Read<T>();
    }

    template <typename T> requires ((std::integral<T> || std::same_as<T, float>) && sizeof(T) <= 4)
    auto SlPacketReader::Peek() const -> T
    {
        const BufferReader& reader = _readers.at(_index);

        constexpr uint8_t expected = detail::PacketValueTraits<T>::type_value;
        ThrowIfInvalidRead(expected, reader.Peek<uint8_t>());

        std::array<char, sizeof(uint8_t) + sizeof(T)> buffer;
        reader.Peek(buffer);

        return *reinterpret_cast<const T*>(buffer.data() + 1);
    }

    template <typename T> requires std::is_enum_v<T>
    auto SlPacketReader::Read() -> T
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        constexpr uint8_t expected = detail::PacketValueTraits<std::underlying_type_t<T>>::type_value;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        T result = {};
        reader.Read<T>(result);

        return result;
    }

    template <typename T> requires std::is_enum_v<T>
    auto SlPacketReader::Peek() const -> T
    {
        return static_cast<T>(Peek<std::underlying_type_t<T>>());
    }
}

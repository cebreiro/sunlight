#pragma once
#include "sl/emulator/server/packet/io/sl_packet_value_traits.h"

namespace sunlight
{
    // sub_58DA70
    class SlPacketWriter
    {
    public:
        template <typename T> requires ((std::integral<T> || std::same_as<T, float>) && sizeof(T) <= 4)
        void Write(T value);

        template <typename T> requires std::is_enum_v<T>
        void Write(T value);

        void WriteString(const std::string& str);

        void WriteObject(std::span<const char> buffer);
        void WriteObject(PacketWriter& objectWriter);
        void WriteObject(const Buffer& buffer);
        void WriteZeroBytes(int64_t size);

        auto Flush() const -> Buffer;

    private:
        auto CalculateSize() const -> int64_t;

        static void ThrowIfOverflow(int64_t size);

    private:
        using block_t = boost::container::small_vector<uint8_t, 64>;
        boost::container::small_vector<block_t, 16> _blocks;

        int64_t _size = 0;
    };

    template <typename T> requires std::is_enum_v<T>
    void SlPacketWriter::Write(T value)
    {
        this->Write(static_cast<std::underlying_type_t<T>>(value));
    }

    template <typename T> requires ((std::integral<T> || std::same_as<T, float>) && sizeof(T) <= 4)
    void SlPacketWriter::Write(T value)
    {
        block_t& block = _blocks.emplace_back();

        std::array<uint8_t, sizeof(T) + 1> buffer = {};

        buffer[0] = detail::PacketValueTraits<T>::type_value;
        *reinterpret_cast<T*>(buffer.data() + 1) = value;

        std::copy_n(buffer.begin(), sizeof(buffer), std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }
}

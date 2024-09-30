#pragma once
#include "shared/network/buffer/buffer_writer.h"

namespace sunlight
{
    class PacketWriter final
    {
    public:
        PacketWriter(const PacketWriter& other) = delete;
        PacketWriter(PacketWriter&& other) noexcept = delete;
        PacketWriter& operator=(const PacketWriter& other) = delete;
        PacketWriter& operator=(PacketWriter&& other) noexcept = delete;

        PacketWriter();
        ~PacketWriter();

        template <std::integral T>
        void Write(T value);
        void WriteString(const std::string& str);
        void WriteBuffer(std::span<const char> buffer);
        void WriteZeroBytes(int64_t count);

        void WriteFixeSizeString(const std::string& str, int64_t size);

        template <std::integral T>
        void Write(T value, int64_t offset);

        void Write(const IBufferSerializable& serializable);

        template <typename T> requires std::is_enum_v<T>
        void Write(T value);

        template <std::floating_point T>
        void Write(T value);

        template <std::ranges::input_range T>
        void Write(T range);

        auto Flush() -> Buffer;

        auto GetBuffer() -> Buffer&;
        auto GetBuffer() const -> const Buffer&;
        auto GetWriteSize() const->int64_t;

    private:
        Buffer _buffer;
        BufferWriter _writer;
    };

    template <std::integral U>
    void PacketWriter::Write(U value)
    {
        _writer.Write(value);
    }

    template <std::integral U>
    void PacketWriter::Write(U value, int64_t offset)
    {
        _writer.Write(value, offset);
    }

    template <typename T> requires std::is_enum_v<T>
    void PacketWriter::Write(T value)
    {
        _writer.Write(value);
    }

    template <std::floating_point U>
    void PacketWriter::Write(U value)
    {
        _writer.Write(value);
    }

    template <std::ranges::input_range T>
    void PacketWriter::Write(T range)
    {
        _writer.Write(std::move(range));
    }
}

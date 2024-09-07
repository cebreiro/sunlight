#pragma once
#include "shared/io/stream_writer.h"
#include "shared/network/buffer/buffer.h"

namespace sunlight
{
    namespace detail
    {
        class BufferStreamAdapter
        {
        public:
            using value_type = char;

        public:
            BufferStreamAdapter(Buffer& buffer);

            void push_back(value_type c);
            auto size() const -> size_t;

            auto begin() -> Buffer::iterator;

        private:
            Buffer& _buffer;
            Buffer::iterator _begin;
            Buffer::iterator _iter;
            Buffer::iterator _end;
        };
    }

    class IBufferSerializable;

    class BufferWriter
    {
    public:
        BufferWriter() = delete;
        BufferWriter(const BufferWriter& other) = delete;
        BufferWriter(BufferWriter&& other) noexcept = delete;
        BufferWriter& operator=(const BufferWriter& other) = delete;
        BufferWriter& operator=(BufferWriter&& other) noexcept = delete;

        explicit BufferWriter(Buffer& buffer);

        template <std::integral U>
        void Write(U value);
        void WriteString(const std::string& str);
        void WriteBuffer(std::span<const char> buffer);

        template <std::integral U>
        void Write(U value, int64_t offset);

        void Write(const IBufferSerializable& serializable);

        template <typename T> requires std::is_enum_v<T>
        void Write(T value);

        template <std::floating_point U>
        void Write(U value);

        template <std::ranges::input_range T>
        void Write(T range);

        auto GetWriteSize() const -> int64_t;

    private:
        Buffer& _buffer;
        detail::BufferStreamAdapter _adapter;
        StreamWriter<detail::BufferStreamAdapter> _streamWriter;
    };

    template <std::integral U>
    void BufferWriter::Write(U value)
    {
        _streamWriter.Write(value);
    }

    template <std::integral U>
    void BufferWriter::Write(U value, int64_t offset)
    {
        _streamWriter.Write(value, offset);
    }

    template <typename T> requires std::is_enum_v<T>
    void BufferWriter::Write(T value)
    {
        using underlying_type = std::underlying_type_t<T>;

        Write<underlying_type>(static_cast<underlying_type>(value));
    }

    template <std::floating_point U>
    void BufferWriter::Write(U value)
    {
        _streamWriter.Write(value);
    }

    template <std::ranges::input_range T>
    void BufferWriter::Write(T range)
    {
        for (const std::ranges::range_value_t<T>& value : range)
        {
            Write(value);
        }
    }
}

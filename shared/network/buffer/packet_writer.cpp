#include "packet_writer.h"

#include <boost/scope/scope_exit.hpp>

namespace sunlight
{
    PacketWriter::PacketWriter()
        : _writer(_buffer)
    {
    }

    PacketWriter::~PacketWriter()
    {
    }

    void PacketWriter::WriteString(const std::string& str)
    {
        _writer.WriteString(str);
    }

    void PacketWriter::WriteBuffer(std::span<const char> buffer)
    {
        _writer.WriteBuffer(buffer);
    }

    void PacketWriter::WriteZeroBytes(int64_t count)
    {
        char* buffer = static_cast<char*>(_malloca(count));

        boost::scope::scope_exit exit([buffer]()
            {
                _freea(buffer);
            });

        memset(buffer, 0, count);

        _writer.WriteBuffer(std::span(buffer, count));
    }

    void PacketWriter::WriteFixeSizeString(const std::string& str, int64_t size)
    {
        char* buffer = static_cast<char*>(_malloca(size));

        boost::scope::scope_exit exit([buffer]()
            {
                _freea(buffer);
            });

        ::memset(buffer, 0, size);

        std::copy_n(str.data(), std::min(std::ssize(str), size), buffer);

        _writer.WriteBuffer(std::span(buffer, size));
    }

    void PacketWriter::Write(const IBufferSerializable& serializable)
    {
        _writer.Write(serializable);
    }

    auto PacketWriter::Flush() -> Buffer
    {
        Buffer result;

        [[maybe_unused]] const bool sliced = _buffer.SliceFront(result, GetWriteSize());
        assert(sliced);

        return result;
    }

    auto PacketWriter::GetBuffer() -> Buffer&
    {
        return _buffer;
    }

    auto PacketWriter::GetBuffer() const -> const Buffer&
    {
        return _buffer;
    }

    auto PacketWriter::GetWriteSize() const -> int64_t
    {
        return _writer.GetWriteSize();
    }
}

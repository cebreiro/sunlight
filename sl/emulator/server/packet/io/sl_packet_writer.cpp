#include "sl_packet_writer.h"

namespace sunlight
{
    void SlPacketWriter::WriteString(const std::string& str)
    {
        block_t& block = _blocks.emplace_back();

        const int64_t bufferSize = std::ssize(str) + 1;
        ThrowIfOverflow(bufferSize);

        uint16_t size = static_cast<uint16_t>(bufferSize);
        if (!str.empty() && str.back() == '\0')
        {
            size -= 1;
        }

        if (size <= 0x6E)
        {
            block.push_back(uint8_t{ 8 } + static_cast<uint8_t>(size));
        }
        else
        {
            block.push_back(detail::PacketValueTraits<std::string>::type_value);
            std::copy_n(reinterpret_cast<const uint8_t*>(&size), sizeof(uint16_t), std::back_inserter(block));
        }

        std::copy_n(str.data(), size, std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    void SlPacketWriter::WriteObject(std::span<const char> buffer)
    {
        block_t& block = _blocks.emplace_back();

        const int64_t bufferSize = std::ssize(buffer);
        ThrowIfOverflow(bufferSize);

        const uint16_t size = static_cast<uint16_t>(bufferSize);

        if (size <= 0x6E)
        {
            block.push_back(uint8_t{ 0x81 } + static_cast<uint8_t>(size));
        }
        else
        {
            block.push_back(detail::PacketValueTraits<detail::object_type>::type_value);
            std::copy_n(reinterpret_cast<const uint8_t*>(&size), sizeof(uint16_t), std::back_inserter(block));
        }

        std::copy_n(buffer.begin(), size, std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    void SlPacketWriter::WriteObject(PacketWriter& objectWriter)
    {
        block_t& block = _blocks.emplace_back();

        const int64_t bufferSize = objectWriter.GetWriteSize();
        ThrowIfOverflow(bufferSize);

        Buffer buffer = objectWriter.Flush();

        const uint16_t size = static_cast<uint16_t>(bufferSize);

        if (size <= 0x6E)
        {
            block.push_back(uint8_t{ 0x81 } + static_cast<uint8_t>(size));
        }
        else
        {
            block.push_back(detail::PacketValueTraits<detail::object_type>::type_value);
            std::copy_n(reinterpret_cast<const uint8_t*>(&size), sizeof(uint16_t), std::back_inserter(block));
        }

        std::copy_n(buffer.begin(), size, std::back_inserter(block));

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    void SlPacketWriter::WriteZeroBytes(int64_t size)
    {
        if (size <= 0)
        {
            assert(false);

            return;
        }

        ThrowIfOverflow(size);

        block_t& block = _blocks.emplace_back();

        if (size <= 0x6E)
        {
            block.resize(size + 1, 0);
            block[0] = static_cast<uint8_t>(uint8_t{ 0x81 } + size);
        }
        else
        {
            block.resize(size + 3, 0);
            block[0] = detail::PacketValueTraits<detail::object_type>::type_value;
            *reinterpret_cast<uint16_t*>(block.data() + 1) = static_cast<uint16_t>(size);
        }

        _size += std::ssize(block);
        assert(_size == CalculateSize());
    }

    auto SlPacketWriter::Flush() const -> Buffer
    {
        Buffer buffer;

        buffer.Add(buffer::Fragment::Create(_size + 2));
        assert(_size == CalculateSize());

        BufferWriter writer(buffer);
        writer.Write<uint16_t>(static_cast<uint16_t>(_blocks.size()));

        auto range = _blocks | std::views::reverse | std::views::join;

        std::ranges::copy_n(range.begin(), buffer.GetSize() - 2, std::next(buffer.begin(), 2));
        return buffer;
    }

    auto SlPacketWriter::CalculateSize() const -> int64_t
    {
        return std::accumulate(_blocks.begin(), _blocks.end(), int64_t{ 0 },
            [](int64_t sum, const block_t& block) -> int64_t
            {
                return sum + std::ssize(block);
            });
    }

    void SlPacketWriter::ThrowIfOverflow(int64_t size)
    {
        if (size > std::numeric_limits<uint16_t>::max())
        {
            throw std::runtime_error(fmt::format("[{}] fail to write object. too big object. size: {}, stack: {}",
                __FUNCTION__, size, to_string(std::stacktrace::current())));
        }
    }
}

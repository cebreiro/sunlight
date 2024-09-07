#include "sl_packet_reader.h"

namespace sunlight
{
    SlPacketReader::SlPacketReader(const Buffer& buffer)
    {
        Initialize(buffer);
    }

    auto SlPacketReader::ReadInt64() -> std::pair<int32_t, int32_t>
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        constexpr uint8_t expected = 0x81ui8 + 8;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        const int32_t low = reader.Read<int32_t>();
        const int32_t high = reader.Read<int32_t>();

        return std::make_pair(low, high);
    }

    auto SlPacketReader::ReadUInt64() -> std::pair<uint32_t, uint32_t>
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        constexpr uint8_t expected = 0x81ui8 + 8;
        ThrowIfInvalidRead(expected, reader.Read<uint8_t>());

        const uint32_t low = reader.Read<uint32_t>();
        const uint32_t high = reader.Read<uint32_t>();

        return std::make_pair(low, high);
    }

    auto SlPacketReader::ReadString() -> std::string
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        const uint8_t type = reader.Read<uint8_t>();
        const uint16_t size = type == detail::PacketValueTraits<std::string>::type_value
            ? reader.Read<uint16_t>() : type - uint8_t{ 8 };

        return reader.ReadString(size);
    }

    auto SlPacketReader::ReadObject() -> BufferReader
    {
        BufferReader& reader = _readers.at(_index);
        ++_index;

        const uint8_t type = reader.Read<uint8_t>();
        const uint16_t size = type == detail::PacketValueTraits<detail::object_type>::type_value
            ? reader.Read<uint16_t>() : type - uint8_t{ 8 };

        const auto begin = reader.current();
        const auto end = std::next(begin, size);

        return BufferReader(begin, end);
    }

    void SlPacketReader::ReadObject(std::vector<char>& result)
    {
        const BufferReader reader = ReadObject();

        const auto begin = reader.begin();
        const auto end = reader.end();

        result.reserve(std::distance(begin, end));
        std::copy(begin, end, std::back_inserter(result));
    }

    void SlPacketReader::Initialize(const Buffer& buffer)
    {
        BufferReader reader(buffer.begin(), buffer.end());

        const size_t blockCount = reader.Read<uint16_t>();

        // client 58DA70h
        for (size_t i = 0; i < blockCount; ++i)
        {
            const Buffer::const_iterator current = reader.current();
            _readers.push_back(BufferReader(current, buffer.end()));

            const uint8_t typeValue = reader.Read<uint8_t>();
            if (typeValue == detail::PacketValueTraits<int8_t>::type_value)
            {
                reader.Skip(sizeof(int8_t));
            }
            else if (typeValue == detail::PacketValueTraits<int16_t>::type_value)
            {
                reader.Skip(sizeof(int16_t));
            }
            else if (typeValue == detail::PacketValueTraits<int32_t>::type_value)
            {
                reader.Skip(sizeof(int32_t));
            }
            else if (typeValue == detail::PacketValueTraits<float>::type_value)
            {
                reader.Skip(sizeof(float));
            }
            else if (typeValue == detail::PacketValueTraits<std::string>::type_value ||
                typeValue == detail::PacketValueTraits<detail::object_type>::type_value)
            {
                reader.Skip(reader.Read<uint16_t>());
            }
            else
            {
                if (typeValue > 0x81 && typeValue <= 0xFF)
                {
                    reader.Skip(typeValue - 0x81ui8);
                }
                else if (typeValue > 8 && typeValue <= 0x6E)
                {
                    reader.Skip(typeValue - 8ui8);
                }
                else
                {
                    throw std::runtime_error(
                        std::format("packet_reader found unknown type: {}, buffer: {}",
                            typeValue, buffer.ToString()));
                }
            }
        }

        if (reader.current() != buffer.end())
        {
            throw std::runtime_error(
                std::format("packet_reader parse error. buffer: {}", buffer.ToString()));
        }

        std::ranges::reverse(_readers);
    }

    void SlPacketReader::ThrowIfInvalidRead(uint8_t expected, uint8_t result)
    {
        if (expected != result)
        {
            assert(false);
            throw std::runtime_error("invalid packet read");
        }
    }
}

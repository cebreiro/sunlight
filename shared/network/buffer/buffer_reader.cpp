#include "buffer_reader.h"

#include "shared/network/buffer/buffer_deserializable.h"

namespace sunlight
{
    BufferReader::BufferReader(const value_type& begin, const value_type& end)
        : StreamReader(begin, end)
    {
    }

    void BufferReader::Read(IBufferDeserializable& deserializable)
    {
        deserializable.Deserialize(*this);
    }
}

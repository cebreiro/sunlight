#pragma once

namespace sunlight
{
    class BufferReader;

    class IBufferDeserializable
    {
    public:
        virtual ~IBufferDeserializable() = default;

        virtual void Deserialize(BufferReader& reader) = 0;
    };
}

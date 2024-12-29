#pragma once

namespace sunlight
{
    class BufferWriter;

    class IBufferSerializable
    {
    public:
        virtual ~IBufferSerializable() = default;

        virtual void Serialize(BufferWriter& writer) const = 0;
    };
}

#pragma once

namespace sunlight
{
    class IPacketCodec
    {
    public:
        virtual ~IPacketCodec() = default;

        virtual void Decode(Buffer::iterator begin, Buffer::iterator end) = 0;
        virtual void Encode(Buffer::iterator begin, Buffer::iterator end) = 0;
    };
}

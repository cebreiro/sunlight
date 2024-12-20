#pragma once
#include "sl/generator/server/packet/codec/packet_codec_interface.h"

namespace sunlight
{
    class LoginPacketCodec final : public IPacketCodec
    {
    public:
        LoginPacketCodec(uint32_t key1, uint32_t key2);

        void Decode(Buffer::iterator begin, Buffer::iterator end) override;
        void Encode(Buffer::iterator begin, Buffer::iterator end) override;

    private:
        std::array<uint32_t, 2> _encodeKeys = {};
        std::array<uint32_t, 2> _decodeKeys = {};
    };
}

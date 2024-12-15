#pragma once
#include "sl/generator/server/packet/codec/packet_codec_interface.h"

namespace sunlight
{
    class ZonePacketCodec final : public IPacketCodec
    {
    public:
        void Decode(Buffer::iterator begin, Buffer::iterator end) override;
        void Encode(Buffer::iterator begin, Buffer::iterator end) override;

    private:
        int32_t _decodeSN = 0;
        int32_t _encodeSN = 0;

        int8_t _decodeLastValue = 0;
        int8_t _encodeLastValue = 0;
    };
}

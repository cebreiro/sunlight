#include "zone_packet_codec.h"

#include "sl/emulator/server/server_constant.h"

namespace sunlight
{
    void ZonePacketCodec::Decode(Buffer::iterator begin, Buffer::iterator end)
    {
        int32_t i = 0;
        char prev = _decodeLastValue;

        for (auto iter = begin; iter != end; ++iter, ++i)
        {
            *iter = (*iter ^ static_cast<char>(ServerConstant::CRYPTO_VALUES[(i + _decodeSN) & 0xFF])) - prev;
            prev = *iter;
        }

        _decodeSN += i;
        _decodeLastValue = prev;
    }

    void ZonePacketCodec::Encode(Buffer::iterator begin, Buffer::iterator end)
    {
        char temp1 = 0;
        char temp2 = _encodeLastValue;

        int64_t i = 0;

        for (auto iter = begin; iter != end; ++iter, ++i)
        {
            char& c = *iter;

            temp1 = c;
            c = (temp2 + c) ^ static_cast<char>(ServerConstant::CRYPTO_VALUES[(i + _encodeSN) & 0xFF]);

            temp2 = temp1;
        }

        _encodeSN += i & 0xFF;
        _encodeLastValue = temp2;
    }
}

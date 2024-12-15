#include "lobby_packet_codec.h"

#include "sl/generator/server/server_constant.h"

namespace sunlight
{
    void LobbyPacketCodec::Decode(Buffer::iterator begin, Buffer::iterator end)
    {
        int32_t i = 0;
        char prev = 0;

        for (auto iter = begin; iter != end; ++iter, ++i)
        {
            *iter = (*iter ^ static_cast<char>(ServerConstant::CRYPTO_VALUES[(i + _decodeSN) & 0xFF])) - prev;
            prev = *iter;
        }

        _decodeSN += i;
    }

    void LobbyPacketCodec::Encode(Buffer::iterator begin, Buffer::iterator end)
    {
        auto iter = begin;

        char CL = 0;
        char BL = *iter;

        *iter = BL ^ static_cast<char>(ServerConstant::CRYPTO_VALUES[(_encodeSN & 0xFF)]);

        ++iter;
        int32_t i = 1;

        for (; iter != end; ++iter, ++i)
        {
            char& c = *iter;

            CL = c;
            CL = CL + BL;
            BL = c;
            c = CL ^ static_cast<char>(ServerConstant::CRYPTO_VALUES[(i + _encodeSN) & 0xFF]);
        }

        _encodeSN += i & 0xFF;
    }
}

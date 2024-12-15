#include "login_packet_codec.h"

namespace sunlight
{
    LoginPacketCodec::LoginPacketCodec(uint32_t key1, uint32_t key2)
        : _encodeKeys({ key1, key2 })
        , _decodeKeys({ key1, key2 })
    {
        if (_encodeKeys[0] >= 0x80000000)
        {
            _encodeKeys[1] = 0xFFFFFFFF;
        }

        if (_decodeKeys[0] >= 0x80000000)
        {
            _decodeKeys[1] = 0xFFFFFFFF;
        }
    }

    void LoginPacketCodec::Decode(Buffer::iterator begin, Buffer::iterator end)
    {
        auto iter = begin;
        uint32_t i = 0;

        if (_decodeKeys[0] | _decodeKeys[1])
        {
            const char* key = reinterpret_cast<const char*>(_decodeKeys.data());

            char AL = *iter;
            *iter = AL ^ key[i];

            ++iter;
            ++i;

            for (; iter != end; ++iter, ++i)
            {
                char& c = *iter;

                char BL = c ^ (AL ^ key[i & 7]);
                AL = c;
                c = BL;
            }

            _decodeKeys[0] += i;

            if (_decodeKeys[0] >= 0x80000000)
            {
                _decodeKeys[1] = 0xFFFFFFFF;
            }
        }
    }

    void LoginPacketCodec::Encode(Buffer::iterator begin, Buffer::iterator end)
    {
        if (_encodeKeys[0] | _encodeKeys[1])
        {
            const char* key = reinterpret_cast<const char*>(_encodeKeys.data());

            uint32_t i = 0;
            auto iter = begin;

            *iter = (*iter ^ key[i]);
            char CL = *iter;

            ++iter;
            ++i;

            for (; iter != end; ++iter, ++i)
            {
                char& c = *iter;

                char BL = CL ^ (c ^ key[i & 7]);
                c = BL;
                CL = c;
            }

            _encodeKeys[0] += i;

            if (_encodeKeys[0] >= 0x80000000)
            {
                _encodeKeys[1] = 0xFFFFFFFF;
            }
        }
    }
}

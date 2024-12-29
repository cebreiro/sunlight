#pragma once

namespace sunlight
{
    class ServerConstant
    {
    public:
        ServerConstant() = delete;

        static constexpr uint64_t EPOCH = 1672498800000;

        static constexpr const char* TEXT_ENCODING = "EUC-KR";

        static constexpr uint16_t LOGIN_PORT = 2106;
        static constexpr uint16_t LOBBY_PORT = 2000;
        static const std::array<uint8_t, 256> CRYPTO_VALUES;

        static const uint32_t LOGIN_FIRST_KEY;
        static const uint32_t LOGIN_SECOND_KEY;

        static constexpr int32_t GAME_VERSION = 2;
    };
}

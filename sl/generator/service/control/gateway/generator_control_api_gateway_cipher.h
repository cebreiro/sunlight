#pragma once
#include <openssl/types.h>

namespace sunlight
{
    class GeneratorControlAPIGatewayCipher
    {
    public:
        static constexpr int32_t TAG_SIZE = 16;

    public:
        GeneratorControlAPIGatewayCipher() = delete;
        GeneratorControlAPIGatewayCipher(const GeneratorControlAPIGatewayCipher& other) = delete;
        GeneratorControlAPIGatewayCipher(GeneratorControlAPIGatewayCipher&& other) noexcept = delete;
        GeneratorControlAPIGatewayCipher& operator=(const GeneratorControlAPIGatewayCipher& other) = delete;
        GeneratorControlAPIGatewayCipher& operator=(GeneratorControlAPIGatewayCipher&& other) noexcept = delete;

        explicit GeneratorControlAPIGatewayCipher(const std::string& key);
        ~GeneratorControlAPIGatewayCipher();

        bool Encrypt(Buffer& buffer, std::span<const char> additionalData, std::array<char, TAG_SIZE>& tag);
        bool Decrypt(Buffer& buffer, std::span<const char> additionalData, std::array<char, TAG_SIZE>& tag);

    private:
        static void Increase(std::array<char, 12>& nonce);

    private:
        EVP_CIPHER_CTX* _ctx = nullptr;

        std::array<char, 32> _key = {};
        std::array<char, 12> _encryptNonce = {};
        std::array<char, 12> _decryptNonce = {};
    };
}

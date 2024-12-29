#include "generator_control_api_gateway_cipher.h"

#include <openssl/evp.h>

namespace sunlight
{
    GeneratorControlAPIGatewayCipher::GeneratorControlAPIGatewayCipher(const std::string& key)
        : _ctx(EVP_CIPHER_CTX_new())
    {
        std::copy_n(key.begin(), std::min(std::ssize(key), std::ssize(_key)), _key.begin());
    }

    GeneratorControlAPIGatewayCipher::~GeneratorControlAPIGatewayCipher()
    {
        if (_ctx)
        {
            EVP_CIPHER_CTX_free(_ctx);
        }
    }

    bool GeneratorControlAPIGatewayCipher::Encrypt(Buffer& buffer, std::span<const char> additionalData, std::array<char, TAG_SIZE>& tag)
    {
        Increase(_encryptNonce);

        const unsigned char* key = reinterpret_cast<const unsigned char*>(_key.data());
        const unsigned char* nonce = reinterpret_cast<const unsigned char*>(_encryptNonce.data());

        if (!EVP_EncryptInit_ex(_ctx, EVP_chacha20_poly1305(), nullptr, key, nonce))
        {
            return false;
        }

        if (!additionalData.empty())
        {
            const unsigned char* ad = reinterpret_cast<const unsigned char*>(additionalData.data());
            const int32_t adSize = static_cast<int32_t>(std::ssize(additionalData));

            int32_t length = 0;

            if (!EVP_EncryptUpdate(_ctx, nullptr, &length, ad, adSize))
            {
                return false;
            }

            assert(length == adSize);
        }

        for (buffer::Fragment& fragment : buffer.GetFragmentContainer())
        {
            unsigned char* data = reinterpret_cast<unsigned char*>(fragment.GetData());
            const int32_t size = static_cast<int32_t>(fragment.GetSize());

            int32_t length = 0;

            if (!EVP_EncryptUpdate(_ctx, data, &length, data, size))
            {
                return false;
            }

            assert(length == size);
        }

        int32_t length = 0;

        if (!EVP_EncryptFinal_ex(_ctx, nullptr, &length))
        {
            return false;
        }

        if (!EVP_CIPHER_CTX_ctrl(_ctx, EVP_CTRL_AEAD_GET_TAG, TAG_SIZE, tag.data()))
        {
            return false;
        }

        return true;
    }

    bool GeneratorControlAPIGatewayCipher::Decrypt(Buffer& buffer, std::span<const char> additionalData, std::array<char, TAG_SIZE>& tag)
    {
        Increase(_decryptNonce);

        const unsigned char* key = reinterpret_cast<const unsigned char*>(_key.data());
        const unsigned char* nonce = reinterpret_cast<const unsigned char*>(_decryptNonce.data());

        if (!EVP_DecryptInit_ex(_ctx, EVP_chacha20_poly1305(), nullptr, key, nonce))
        {
            return false;
        }

        if (!additionalData.empty())
        {
            const unsigned char* ad = reinterpret_cast<const unsigned char*>(additionalData.data());
            const int32_t adSize = static_cast<int32_t>(std::ssize(additionalData));

            int32_t length = 0;

            if (!EVP_DecryptUpdate(_ctx, nullptr, &length, ad, adSize))
            {
                return false;
            }

            assert(length == adSize);
        }

        for (buffer::Fragment& fragment : buffer.GetFragmentContainer())
        {
            unsigned char* data = reinterpret_cast<unsigned char*>(fragment.GetData());
            const int32_t size = static_cast<int32_t>(fragment.GetSize());

            int32_t length = 0;

            if (!EVP_DecryptUpdate(_ctx, data, &length, data, size))
            {
                return false;
            }

            assert(length == size);
        }

        if (!EVP_CIPHER_CTX_ctrl(_ctx, EVP_CTRL_AEAD_SET_TAG, TAG_SIZE, tag.data()))
        {
            return false;
        }

        int32_t length = 0;

        if (!EVP_DecryptFinal_ex(_ctx, nullptr, &length))
        {
            return false;
        }

        return true;
    }

    void GeneratorControlAPIGatewayCipher::Increase(std::array<char, 12>& nonce)
    {
        for (int32_t i = 11; i >= 8; --i)
        {
            if (++nonce[i] != 0)
            {
                break;
            }
        }
    }
}

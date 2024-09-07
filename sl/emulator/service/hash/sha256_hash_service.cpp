#include "sha256_hash_service.h"

#include <openssl/evp.h>
#include <openssl/sha.h>

namespace sunlight
{
    Sha256HashService::Sha256HashService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , evpContext(EVP_MD_CTX_new())
    {
    }

    Sha256HashService::~Sha256HashService()
    {
        EVP_MD_CTX_free(static_cast<EVP_MD_CTX*>(evpContext));
    }

    auto Sha256HashService::GetName() const -> std::string_view
    {
        return "sha256_hash_service";
    }

    auto Sha256HashService::Hash(const char* buffer, size_t length) -> Future<std::string>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        EVP_MD_CTX* context = static_cast<EVP_MD_CTX*>(evpContext);
        const EVP_MD* md = EVP_sha256();
        assert(md);

        [[maybe_unused]]
        const int32_t initResult = EVP_DigestInit_ex(context, md, nullptr);
        assert(initResult == 1);

        [[maybe_unused]]
        const int32_t updateResult = EVP_DigestUpdate(context, buffer, length);
        assert(updateResult == 1);

        std::ostringstream oss;

        unsigned char hash[SHA256_DIGEST_LENGTH] = {};
        uint32_t hashLength = 0;

        [[maybe_unused]]
        const int32_t finalResult = EVP_DigestFinal_ex(context, hash, &hashLength);
        assert(finalResult == 1);

        for (uint32_t i = 0; i < hashLength; i++)
        {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int32_t>(hash[i]);
        }

        co_return oss.str();
    }
}

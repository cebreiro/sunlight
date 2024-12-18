#include "safe_hash_service.h"

#include <argon2.h>

namespace sunlight
{
    SafeHashService::SafeHashService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _mt(std::random_device{}())
    {
    }

    auto SafeHashService::GetName() const -> std::string_view
    {
        return "safe_hash_service";
    }

    auto SafeHashService::Hash(std::string pwd) -> Future<std::string>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        constexpr int32_t timeCost = 2;
        constexpr int32_t memoryCost = 0xFFFF;
        constexpr int32_t parallelism = 1;

        constexpr int64_t hashSize = 32;
        constexpr int64_t encodedSize = 128;

        std::array<char, encodedSize> encoded = {};
        const std::array<char, 16> salt = MakeSalt();

        if (argon2id_hash_encoded(
            timeCost,
            memoryCost,
            parallelism,
            pwd.c_str(), std::ssize(pwd),
            salt.data(), std::ssize(salt),
            hashSize,
            encoded.data(), std::ssize(encoded)) != ARGON2_OK)
        {
            SUNLIGHT_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to hash encode. str: {}, salt: {}",
                    GetName(), pwd, std::string(salt.begin(), salt.end())));

            co_return std::string{};
        }

        co_return std::string(encoded.begin(), encoded.end());
    }

    auto SafeHashService::Compare(std::string hashed, std::string raw) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        co_return argon2id_verify(hashed.c_str(), raw.c_str(), raw.size()) == ARGON2_OK;
    }

    auto SafeHashService::MakeSalt() -> std::array<char, 16>
    {
        constexpr int32_t min = 'a';
        constexpr int32_t max = 'z';

        std::uniform_int_distribution<int32_t> dist(min, max);

        std::array<char, 16> result;

        for (int64_t i = 0; i < std::ssize(result); ++i)
        {
            result[i] = static_cast<char>(dist(_mt));
        }

        return result;
    }
}

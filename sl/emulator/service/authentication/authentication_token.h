#pragma once
#include "sl/emulator/server/client/game_client_id.h"

namespace sunlight
{
    class AuthenticationToken
    {
    public:
        class Key
        {
        public:
            Key(int32_t low, int32_t high);

            auto GetLow() const -> int32_t;
            auto GetHigh() const -> int32_t;

            auto ToString() const -> std::string;

            friend bool operator==(const Key& lhs, const Key& rhs);
            friend bool operator!=(const Key& lhs, const Key& rhs);

        private:
            int32_t _low = 0;
            int32_t _high = 0;
        };

    public:
        AuthenticationToken() = delete;
        AuthenticationToken(const AuthenticationToken& other) = delete;
        AuthenticationToken(AuthenticationToken&& other) noexcept = delete;
        AuthenticationToken& operator=(const AuthenticationToken& other) = delete;
        AuthenticationToken& operator=(AuthenticationToken&& other) noexcept = delete;

    public:
        AuthenticationToken(const Key& key, int64_t accountId, game_client_id_type clientId);
        ~AuthenticationToken() = default;

        auto GetKey() const -> const Key&;
        auto GetAccountId() const -> int64_t;
        auto GetClientId() const -> game_client_id_type;
        auto GetSid() const -> int8_t;

        void SetSid(int8_t sid);

    private:
        Key _key;
        int64_t _accountId;
        game_client_id_type _clientId = {};

        int8_t _sid = 0;
    };
}

namespace fmt
{
    template <>
    struct formatter<sunlight::AuthenticationToken> : formatter<std::string>
    {
        auto format(const sunlight::AuthenticationToken& token, format_context& context) const
            -> format_context::iterator
        {
            return formatter<std::string>::format(
                fmt::format("{{ key: {}, account_id: {}, client_id: {} }}",
                    token.GetKey().ToString(), token.GetAccountId(), token.GetClientId()), context);
        }
    };
}

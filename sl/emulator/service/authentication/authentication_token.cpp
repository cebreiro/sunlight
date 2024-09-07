#include "authentication_token.h"

#include <boost/lexical_cast.hpp>

namespace sunlight
{
    AuthenticationToken::Key::Key(uint32_t low, uint32_t high)
        : _low(low)
        , _high(high)
    {
    }

    auto AuthenticationToken::Key::GetLow() const -> uint32_t
    {
        return _low;
    }

    auto AuthenticationToken::Key::GetHigh() const -> uint32_t
    {
        return _high;
    }

    auto AuthenticationToken::Key::ToString() const -> std::string
    {
        return fmt::format("{}:{}", _low, _high);
    }

    auto AuthenticationToken::Key::From(const std::string& str) -> std::optional<Key>
    {
        const size_t i = str.find_first_of(":");
        if (i == std::string::npos || i == str.size())
        {
            return std::nullopt;
        }

        try
        {
            const std::string_view left(str.data(), i);
            const std::string_view right(str.begin() + static_cast<int64_t>(i) + 1, str.end());

            const auto low = boost::lexical_cast<uint32_t>(left);
            const auto high = boost::lexical_cast<uint32_t>(right);

            return Key(low, high);
        }
        catch (...)
        {
        }

        return std::nullopt;
    }

    AuthenticationToken::AuthenticationToken(const Key& key, int64_t accountId, game_client_id_type clientId)
        : _key(key)
        , _accountId(accountId)
        , _clientId(clientId)
    {
    }

    auto AuthenticationToken::GetKey() const -> const Key&
    {
        return _key;
    }

    auto AuthenticationToken::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    auto AuthenticationToken::GetClientId() const -> game_client_id_type
    {
        return _clientId;
    }

    auto AuthenticationToken::GetSid() const -> int8_t
    {
        return _sid;
    }

    void AuthenticationToken::SetSid(int8_t sid)
    {
        _sid = sid;
    }

    bool operator==(const AuthenticationToken::Key& lhs, const AuthenticationToken::Key& rhs)
    {
        return lhs._low == rhs._low && lhs._high == rhs._high;
    }

    bool operator!=(const AuthenticationToken::Key& lhs, const AuthenticationToken::Key& rhs)
    {
        return !(lhs == rhs);
    }
}

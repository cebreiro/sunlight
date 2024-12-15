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

    void AuthenticationToken::SetPlayerName(std::string name)
    {
        _playerName = std::move(name);
    }

    bool operator==(const AuthenticationToken::Key& lhs, const AuthenticationToken::Key& rhs)
    {
        return lhs._low == rhs._low && lhs._high == rhs._high;
    }

    bool operator!=(const AuthenticationToken::Key& lhs, const AuthenticationToken::Key& rhs)
    {
        return !(lhs == rhs);
    }

    auto AuthenticationToken::GetPlayerName() const -> const std::string&
    {
        return _playerName;
    }
}

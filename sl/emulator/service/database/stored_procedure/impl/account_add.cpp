#include "account_add.h"

namespace sunlight::db::sp
{
    AccountAdd::AccountAdd(ConnectionPool::Borrowed& conn, std::string account, std::string password)
        : StoredProcedure(conn)
        , _account(std::move(account))
        , _password(std::move(password))
    {
    }

    auto AccountAdd::GetResult() const -> const std::optional<dto::Account>&
    {
        return _result;
    }

    auto AccountAdd::GetSQL() const -> std::string_view
    {
        return "CALL account_add(?, ?)";
    }

    auto AccountAdd::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_account);
        inputs.emplace_back(_password);

        return inputs;
    }

    void AccountAdd::SetOutput(const boost::mysql::results& result)
    {
        if (result.rows().empty())
        {
            return;
        }

        const boost::mysql::row_view& front = result.rows().front();

        size_t index = 0;
        dto::Account& dto = _result.emplace();

        dto.id = front.at(index++).as_int64();
        dto.account = front.at(index++).as_string();
        dto.password = front.at(index++).as_string();
        dto.gmLevel = static_cast<int8_t>(front.at(index++).as_int64());
        dto.banned = static_cast<int8_t>(front.at(index++).as_int64());
        dto.deleted = static_cast<int8_t>(front.at(index++).as_int64());
    }
}

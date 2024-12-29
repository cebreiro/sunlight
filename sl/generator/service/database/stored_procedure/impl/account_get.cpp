#include "account_get.h"

namespace sunlight::db::sp
{
    AccountGet::AccountGet(ConnectionPool::Borrowed& conn, std::string account)
        : StoredProcedure(conn)
        , _account(std::move(account))
    {
    }

    auto AccountGet::GetResult() const -> const std::optional<dto::Account>&
    {
        return _result;
    }

    auto AccountGet::GetSQL() const -> std::string_view
    {
        return "CALL account_get(?)";
    }

    auto AccountGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_account);

        return inputs;
    }

    void AccountGet::SetOutput(const boost::mysql::results& result)
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

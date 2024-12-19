#include "account_password_change.h"

namespace sunlight::db::sp
{
    AccountPasswordChange::AccountPasswordChange(ConnectionPool::Borrowed& conn, std::string account, std::string password)
        : StoredProcedure(conn)
        , _account(std::move(account))
        , _password(std::move(password))
    {
    }

    auto AccountPasswordChange::GetSQL() const -> std::string_view
    {
        return "CALL account_password_change(?, ?)";
    }

    auto AccountPasswordChange::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_account);
        inputs.emplace_back(_password);

        return inputs;
    }

    void AccountPasswordChange::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

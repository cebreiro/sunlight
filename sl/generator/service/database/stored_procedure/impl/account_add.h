#pragma once
#include "sl/generator/service/database/dto/account.h"
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class AccountAdd final : public StoredProcedure
    {
    public:
        AccountAdd(ConnectionPool::Borrowed& conn,
            std::string account, std::string password);

        auto GetResult() const -> const std::optional<dto::Account>&;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const->boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        std::string _account;
        std::string _password;

        std::optional<dto::Account> _result;
    };
}

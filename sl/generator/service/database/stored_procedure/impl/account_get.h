#pragma once
#include "sl/generator/service/database/dto/account.h"
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class AccountGet final : public StoredProcedure
    {
    public:
        AccountGet(ConnectionPool::Borrowed& conn, std::string account);

        auto GetResult() const -> const std::optional<dto::Account>&;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        std::string _account;
        std::optional<dto::Account> _result;
    };
}

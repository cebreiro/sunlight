#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"
#include "sl/generator/service/database/dto/account_storage.h"

namespace sunlight::db::sp
{
    class AccountStorageGet final : public StoredProcedure
    {
    public:
        AccountStorageGet(ConnectionPool::Borrowed& conn, int64_t aid);

        auto GetResult() -> dto::AccountStorage&;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _aid = 0;
        dto::AccountStorage _result;
    };
}

#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db
{
    struct ItemTransaction;
}

namespace sunlight::db::sp
{
    class ItemTransactionExecute final : public StoredProcedure
    {
    public:
        ItemTransactionExecute(ConnectionPool::Borrowed& conn, const ItemTransaction& transaction);

    private:
        auto GetSQL() const->std::string_view override;
        auto GetInput() const->boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        std::string _json;
    };
}

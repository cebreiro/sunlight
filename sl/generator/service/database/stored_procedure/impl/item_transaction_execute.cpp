#include "item_transaction_execute.h"

#include "sl/generator/service/database/transaction/item/item_transaction.h"

namespace sunlight::db::sp
{
    ItemTransactionExecute::ItemTransactionExecute(ConnectionPool::Borrowed& conn, const ItemTransaction& transaction)
        : StoredProcedure(conn)
        , _json([&transaction]() -> std::string
            {
                nlohmann::json j;
                to_json(j, transaction.logs);

                return j.dump();
            }())
    {
    }

    auto ItemTransactionExecute::GetSQL() const -> std::string_view
    {
        return "CALL item_transaction_execute(?)";
    }

    auto ItemTransactionExecute::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_json);

        return inputs;
    }

    void ItemTransactionExecute::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

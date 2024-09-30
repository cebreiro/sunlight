#include "account_storage_get.h"

namespace sunlight::db::sp
{
    AccountStorageGet::AccountStorageGet(ConnectionPool::Borrowed& conn, int64_t aid)
        : StoredProcedure(conn)
        , _aid(aid)
    {
        _result.aid = _aid;
    }

    auto AccountStorageGet::GetResult() -> dto::AccountStorage&
    {
        return _result;
    }

    auto AccountStorageGet::GetSQL() const -> std::string_view
    {
        return "CALL account_storage_get(?)";
    }

    auto AccountStorageGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_aid);

        return inputs;
    }

    void AccountStorageGet::SetOutput(const boost::mysql::results& result)
    {
        if (result.size() < 2)
        {
            assert(false);

            return;
        }

        const boost::mysql::resultset_view& dataSet = result.at(0);
        if (dataSet.rows().empty())
        {
            assert(false);

            return;
        }

        {
            size_t index = 0;
            const boost::mysql::row_view& row = dataSet.rows().at(0);

            _result.page = static_cast<int8_t>(row.at(index++).as_int64());
            _result.gold = static_cast<int32_t>(row.at(index++).as_int64());
        }


        const boost::mysql::resultset_view& itemSet = result.at(1);

        for (const boost::mysql::row_view& row : itemSet.rows())
        {
            size_t index = 0;

            dto::AccountStorage::Item& item = _result.items.emplace_back();

            item.id = row.at(index++).as_int64();
            item.dataId = static_cast<int32_t>(row.at(index++).as_int64());
            item.quantity = static_cast<int32_t>(row.at(index++).as_int64());
            item.page = static_cast<int8_t>(row.at(index++).as_int64());
            item.x = static_cast<int8_t>(row.at(index++).as_int64());
            item.y = static_cast<int8_t>(row.at(index++).as_int64());
        }
    }
}

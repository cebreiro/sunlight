#include "characters_name_get.h"

#include <boost/locale.hpp>

#include "sl/generator/server/server_constant.h"

namespace sunlight::db::sp
{
    CharactersNameGet::CharactersNameGet(ConnectionPool::Borrowed& conn)
        : StoredProcedure(conn)
    {
    }

    auto CharactersNameGet::GetResult() -> std::vector<std::string>&
    {
        return _result;
    }

    auto CharactersNameGet::GetSQL() const -> std::string_view
    {
        return "CALL characters_name_get()";
    }

    auto CharactersNameGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;

        return inputs;
    }

    void CharactersNameGet::SetOutput(const boost::mysql::results& result)
    {
        const size_t size = result.size();
        if (size < 1)
        {
            assert(false);

            return;
        }

        const boost::mysql::resultset_view& resultSet = result.at(0);
        if (resultSet.rows().empty())
        {
            return;
        }

        _result.reserve(result.size());

        for (const boost::mysql::row_view& row : resultSet.rows())
        {
            const std::string& u8str = row.at(0).as_string();

            _result.emplace_back(boost::locale::conv::from_utf(u8str, ServerConstant::TEXT_ENCODING));
        }
    }
}

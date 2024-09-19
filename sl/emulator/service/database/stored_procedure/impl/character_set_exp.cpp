#include "character_set_exp.h"

namespace sunlight::db::sp
{
    CharacterSetExp::CharacterSetExp(ConnectionPool::Borrowed& conn, int64_t cid, int32_t exp)
        : StoredProcedure(conn)
        , _cid(cid)
        , _exp(exp)
    {
    }

    auto CharacterSetExp::GetSQL() const -> std::string_view
    {
        return "CALL character_set_exp(?, ?)";
    }

    auto CharacterSetExp::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_exp);

        return inputs;
    }

    void CharacterSetExp::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

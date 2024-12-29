#include "character_exp_set.h"

namespace sunlight::db::sp
{
    CharacterExpSet::CharacterExpSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t exp)
        : StoredProcedure(conn)
        , _cid(cid)
        , _exp(exp)
    {
    }

    auto CharacterExpSet::GetSQL() const -> std::string_view
    {
        return "CALL character_exp_set(?, ?)";
    }

    auto CharacterExpSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_exp);

        return inputs;
    }

    void CharacterExpSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

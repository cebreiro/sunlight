#include "character_set_job_exp.h"

namespace sunlight::db::sp
{
    CharacterSetJobExp::CharacterSetJobExp(ConnectionPool::Borrowed& conn, int64_t cid, int32_t job, int32_t exp)
        : StoredProcedure(conn)
        , _cid(cid)
        , _job(job)
        , _exp(exp)
    {
    }

    auto CharacterSetJobExp::GetSQL() const -> std::string_view
    {
        return "CALL character_job_exp_set(?, ?, ?)";
    }

    auto CharacterSetJobExp::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_job);
        inputs.emplace_back(_exp);

        return inputs;
    }

    void CharacterSetJobExp::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

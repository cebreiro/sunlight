#include "character_level_set.h"

namespace sunlight::db::sp
{
    CharacterLevelSet::CharacterLevelSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t level, int32_t statPoint)
        : StoredProcedure(conn)
        , _cid(cid)
        , _level(level)
        , _statPoint(statPoint)
    {
    }

    auto CharacterLevelSet::GetSQL() const -> std::string_view
    {
        return "CALL character_level_set(?, ?, ?)";
    }

    auto CharacterLevelSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_level);
        inputs.emplace_back(_statPoint);

        return inputs;
    }

    void CharacterLevelSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

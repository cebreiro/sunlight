#include "character_stat_set.h"

namespace sunlight::db::sp
{
    CharacterStatSet::CharacterStatSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t statPoint, int32_t str,
        int32_t dex, int32_t accr, int32_t health, int32_t intell, int32_t wis, int32_t will)
        : StoredProcedure(conn)
        , _cid(cid)
        , _statPoint(statPoint)
        , _str(str)
        , _dex(dex)
        , _accr(accr)
        , _health(health)
        , _intell(intell)
        , _wis(wis)
        , _will(will)
    {
    }

    auto CharacterStatSet::GetSQL() const -> std::string_view
    {
        return "CALL character_stat_set(?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    auto CharacterStatSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_statPoint);
        inputs.emplace_back(_str);
        inputs.emplace_back(_dex);
        inputs.emplace_back(_accr);
        inputs.emplace_back(_health);
        inputs.emplace_back(_intell);
        inputs.emplace_back(_wis);
        inputs.emplace_back(_will);

        return inputs;
    }

    void CharacterStatSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

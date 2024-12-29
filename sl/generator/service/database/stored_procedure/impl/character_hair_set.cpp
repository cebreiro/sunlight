#include "character_hair_set.h"

namespace sunlight::db::sp
{
    CharacterHairSet::CharacterHairSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t hair)
        : StoredProcedure(conn)
        , _cid(cid)
        , _hair(hair)
    {
    }

    auto CharacterHairSet::GetSQL() const -> std::string_view
    {
        return "CALL character_hair_set(?, ?)";
    }

    auto CharacterHairSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_hair);

        return inputs;
    }

    void CharacterHairSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

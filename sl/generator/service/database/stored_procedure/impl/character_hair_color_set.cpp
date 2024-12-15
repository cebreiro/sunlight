#include "character_hair_color_set.h"

namespace sunlight::db::sp
{
    CharacterHairColorSet::CharacterHairColorSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t hairColor)
        : StoredProcedure(conn)
        , _cid(cid)
        , _hairColor(hairColor)
    {
    }

    auto CharacterHairColorSet::GetSQL() const -> std::string_view
    {
        return "CALL character_hair_color_set(?, ?)";
    }

    auto CharacterHairColorSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_hairColor);

        return inputs;
    }

    void CharacterHairColorSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

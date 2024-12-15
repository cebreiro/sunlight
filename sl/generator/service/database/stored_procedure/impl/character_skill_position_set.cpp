#include "character_skill_position_set.h"

namespace sunlight::db::sp
{
    CharacterSkillPositionSet::CharacterSkillPositionSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t skillId,
        int8_t page, int8_t x, int8_t y)
        : StoredProcedure(conn)
        , _cid(cid)
        , _skillId(skillId)
        , _page(page)
        , _x(x)
        , _y(y)
    {
    }

    auto CharacterSkillPositionSet::GetSQL() const -> std::string_view
    {
        return "CALL character_skill_position_set(?, ?, ?, ?, ?)";
    }

    auto CharacterSkillPositionSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_skillId);
        inputs.emplace_back(_page);
        inputs.emplace_back(_x);
        inputs.emplace_back(_y);

        return inputs;
    }

    void CharacterSkillPositionSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}


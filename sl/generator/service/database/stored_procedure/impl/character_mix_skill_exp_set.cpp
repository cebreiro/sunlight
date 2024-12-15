#include "character_mix_skill_exp_set.h"

namespace sunlight::db::sp
{
    CharacterMixSkillExpSet::CharacterMixSkillExpSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t skillId, int32_t skillLevel, int32_t exp)
        : StoredProcedure(conn)
        , _cid(cid)
        , _skillId(skillId)
        , _skillLevel(skillLevel)
        , _exp(exp)
    {
    }

    auto CharacterMixSkillExpSet::GetSQL() const -> std::string_view
    {
        return "CALL character_mix_skill_exp_set(?, ?, ?, ?)";
    }

    auto CharacterMixSkillExpSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_skillId);
        inputs.emplace_back(_skillLevel);
        inputs.emplace_back(_exp);

        return inputs;
    }

    void CharacterMixSkillExpSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

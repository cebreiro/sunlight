#include "character_skill_level_set.h"

namespace sunlight::db::sp
{
    CharacterSkillLevelSet::CharacterSkillLevelSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t job,
        int32_t skillPoint, int32_t skillId, int32_t skillLevel)
        : StoredProcedure(conn)
        , _cid(cid)
        , _job(job)
        , _skillPoint(skillPoint)
        , _skillId(skillId)
        , _skillLevel(skillLevel)
    {
    }

    auto CharacterSkillLevelSet::GetSQL() const -> std::string_view
    {
        return "CALL character_skill_level_set(?, ?, ?, ?, ?)";
    }

    auto CharacterSkillLevelSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_job);
        inputs.emplace_back(_skillPoint);
        inputs.emplace_back(_skillId);
        inputs.emplace_back(_skillLevel);

        return inputs;
    }

    void CharacterSkillLevelSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

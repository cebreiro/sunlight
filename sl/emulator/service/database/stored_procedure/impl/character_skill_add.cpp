#include "character_skill_add.h"

namespace sunlight::db::sp
{
    CharacterSkillAdd::CharacterSkillAdd(ConnectionPool::Borrowed& conn, int64_t cid, int32_t job, int32_t skillId, int32_t skillLevel)
        : StoredProcedure(conn)
        , _cid(cid)
        , _job(job)
        , _skillId(skillId)
        , _skillLevel(skillLevel)
    {
    }

    auto CharacterSkillAdd::GetSQL() const -> std::string_view
    {
        return "CALL character_skill_add(?, ?, ?, ?)";
    }

    auto CharacterSkillAdd::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_job);
        inputs.emplace_back(_skillId);
        inputs.emplace_back(_skillLevel);

        return inputs;
    }

    void CharacterSkillAdd::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

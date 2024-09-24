#include "character_set_job_level.h"

namespace sunlight::db::sp
{
    CharacterSetJobLevel::CharacterSetJobLevel(ConnectionPool::Borrowed& conn, int64_t cid, int32_t job, int32_t level,
        int32_t skillPoint, const std::vector<req::SkillCreate>& skills)
        : StoredProcedure(conn)
        , _cid(cid)
        , _job(job)
        , _level(level)
        , _skillPoint(skillPoint)
        , _skills([&]() -> std::string
            {
                nlohmann::json j;
                to_json(j, skills);

                return j.dump();
            }())
    {
    }

    auto CharacterSetJobLevel::GetSQL() const -> std::string_view
    {
        return "CALL character_job_level_set(?, ?, ?, ?, ?)";
    }

    auto CharacterSetJobLevel::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_job);
        inputs.emplace_back(_level);
        inputs.emplace_back(_skillPoint);
        inputs.emplace_back(_skills);

        return inputs;
    }

    void CharacterSetJobLevel::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

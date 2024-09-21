#include "character_add_new_job.h"

namespace sunlight::db::sp
{
    CharacterAddNewJob::CharacterAddNewJob(ConnectionPool::Borrowed& conn, int64_t cid, int32_t job, int32_t jobType,
        int32_t level, int32_t skillPoint, const std::vector<req::SkillCreate>& skills)
        : StoredProcedure(conn)
        , _cid(cid)
        , _job(job)
        , _jobType(jobType)
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

    auto CharacterAddNewJob::GetSQL() const -> std::string_view
    {
        return "CALL character_add_new_job(?, ?, ?, ?, ?, ?)";
    }

    auto CharacterAddNewJob::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_job);
        inputs.emplace_back(_jobType);
        inputs.emplace_back(_level);
        inputs.emplace_back(_skillPoint);
        inputs.emplace_back(_skills);

        return inputs;
    }

    void CharacterAddNewJob::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

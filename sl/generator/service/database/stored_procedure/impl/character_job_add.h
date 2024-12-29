#pragma once
#include "sl/generator/service/database/request/skill_create.h"
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterJobAdd final : public StoredProcedure
    {
    public:
        CharacterJobAdd(ConnectionPool::Borrowed& conn, int64_t cid, int32_t job, int32_t jobType, int32_t level, int32_t skillPoint,
            const std::vector<req::SkillCreate>& skills);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid = 0;
        int32_t _job = 0;
        int32_t _jobType = 0;
        int32_t _level = 0;
        int32_t _skillPoint = 0;
        std::string _skills;
    };
}

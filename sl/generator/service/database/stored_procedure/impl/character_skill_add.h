#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterSkillAdd final : public StoredProcedure
    {
    public:
        CharacterSkillAdd(ConnectionPool::Borrowed& conn, int64_t cid, int32_t job, int32_t skillId, int32_t skillLevel);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid = 0;
        int32_t _job = 0;
        int32_t _skillId = 0;
        int32_t _skillLevel = 0;
    };
}

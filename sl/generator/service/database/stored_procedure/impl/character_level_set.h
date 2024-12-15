#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterLevelSet final : public StoredProcedure
    {
    public:
        CharacterLevelSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t level, int32_t statPoint);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid = 0;
        int32_t _level = 0;
        int32_t _statPoint = 0;
    };
}

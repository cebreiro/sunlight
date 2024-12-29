#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterStatSet final : public StoredProcedure
    {
    public:
        CharacterStatSet(ConnectionPool::Borrowed& conn, int64_t cid, int32_t statPoint,
            int32_t str, int32_t dex, int32_t accr, int32_t health, int32_t intell, int32_t wis, int32_t will);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid = 0;
        int32_t _statPoint = 0;
        int32_t _str = 0;
        int32_t _dex = 0;
        int32_t _accr = 0;
        int32_t _health = 0;
        int32_t _intell = 0;
        int32_t _wis = 0;
        int32_t _will = 0;
    };
}

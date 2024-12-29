#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterSkillPositionSet final : public StoredProcedure
    {
    public:
        CharacterSkillPositionSet(ConnectionPool::Borrowed& conn, int64_t cid,
            int32_t skillId, int8_t page, int8_t x, int8_t y);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid = 0;
        int32_t _skillId = 0;
        int8_t _page = 0;
        int8_t _x = 0;
        int8_t _y = 0;
    };
}

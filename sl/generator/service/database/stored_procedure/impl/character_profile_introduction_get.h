#pragma once
#include "sl/generator/service/database/dto/profile_introduction.h"
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterProfileIntroductionGet final : public StoredProcedure
    {
    public:
        CharacterProfileIntroductionGet(ConnectionPool::Borrowed& conn, int64_t cid);

        auto GetResult() -> std::optional<dto::ProfileIntroduction>&;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const->boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid;

        std::optional<dto::ProfileIntroduction> _result = std::nullopt;
    };
}

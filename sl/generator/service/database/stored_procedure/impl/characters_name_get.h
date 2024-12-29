#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharactersNameGet final : public StoredProcedure
    {
    public:
        explicit CharactersNameGet(ConnectionPool::Borrowed& conn);

        auto GetResult() -> std::vector<std::string>&;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const->boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        std::vector<std::string> _result;
    };
}

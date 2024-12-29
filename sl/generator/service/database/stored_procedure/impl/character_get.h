#pragma once
#include "sl/generator/service/database/dto/character.h"
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterGet final : public StoredProcedure
    {
    public:
        CharacterGet(ConnectionPool::Borrowed& conn, int64_t cid);

        auto Release() -> dto::Character;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid = 0;

        std::optional<dto::Character> _result;
    };
}

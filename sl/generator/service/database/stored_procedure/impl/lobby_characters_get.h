#pragma once
#include "sl/generator/service/database/dto/lobby_character.h"
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class LobbyCharactersGet final : public StoredProcedure
    {
    public:
        LobbyCharactersGet(ConnectionPool::Borrowed& conn, int64_t accountId, int8_t sid);

        inline auto GetResult() const;

    private:
        auto GetSQL() const->std::string_view override;
        auto GetInput() const->boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _accountId = 0;
        int8_t _sid = 0;
        std::unordered_map<int64_t, dto::LobbyCharacter> _results;
    };

    inline auto LobbyCharactersGet::GetResult() const
    {
        return _results | std::views::values;
    }
}

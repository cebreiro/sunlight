#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterProfileSet final : public StoredProcedure
    {
    public:
        CharacterProfileSet(ConnectionPool::Borrowed& conn, int64_t cid,
            int8_t refusePartyInvite, int8_t refuseChannelInvite, int8_t refuseGuildInvite, int8_t privateProfile,
            const std::string& age, const std::string& sex, const std::string& mail, const std::string& message);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid;

        int8_t _refusePartyInvite = 0;
        int8_t _refuseChannelInvite = 0;
        int8_t _refuseGuildInvite = 0;
        int8_t _privateProfile = 0;
        std::string _age;
        std::string _sex;
        std::string _mail;
        std::string _message;
    };
}

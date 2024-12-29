#include "character_profile_set.h"

#include <boost/locale.hpp>

#include "sl/generator/server/server_constant.h"

namespace sunlight::db::sp
{
    CharacterProfileSet::CharacterProfileSet(ConnectionPool::Borrowed& conn, int64_t cid, int8_t refusePartyInvite,
        int8_t refuseChannelInvite, int8_t refuseGuildInvite, int8_t privateProfile, const std::string& age,
        const std::string& sex, const std::string& mail, const std::string& message)
        : StoredProcedure(conn)
        , _cid(cid)
        , _refusePartyInvite(refusePartyInvite)
        , _refuseChannelInvite(refuseChannelInvite)
        , _refuseGuildInvite(refuseGuildInvite)
        , _privateProfile(privateProfile)
        , _age(boost::locale::conv::to_utf<char>(age, ServerConstant::TEXT_ENCODING))
        , _sex(boost::locale::conv::to_utf<char>(sex, ServerConstant::TEXT_ENCODING))
        , _mail(boost::locale::conv::to_utf<char>(mail, ServerConstant::TEXT_ENCODING))
        , _message(boost::locale::conv::to_utf<char>(message, ServerConstant::TEXT_ENCODING))
    {
        
    }

    auto CharacterProfileSet::GetSQL() const -> std::string_view
    {
        return "CALL character_profile_set(?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    auto CharacterProfileSet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_refusePartyInvite);
        inputs.emplace_back(_refuseChannelInvite);
        inputs.emplace_back(_refuseGuildInvite);
        inputs.emplace_back(_privateProfile);
        inputs.emplace_back(_age);
        inputs.emplace_back(_sex);
        inputs.emplace_back(_mail);
        inputs.emplace_back(_message);

        return inputs;
    }

    void CharacterProfileSet::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

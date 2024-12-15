#include "character_profile_introduction_get.h"

#include <boost/locale.hpp>

#include "sl/generator/server/server_constant.h"

namespace sunlight::db::sp
{
    CharacterProfileIntroductionGet::CharacterProfileIntroductionGet(ConnectionPool::Borrowed& conn, int64_t cid)
        : StoredProcedure(conn)
        , _cid(cid)
    {
    }

    auto CharacterProfileIntroductionGet::GetResult() -> std::optional<dto::ProfileIntroduction>&
    {
        return _result;
    }

    auto CharacterProfileIntroductionGet::GetSQL() const -> std::string_view
    {
        return "CALL character_profile_introduction_get(?)";
    }

    auto CharacterProfileIntroductionGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);

        return inputs;
    }

    void CharacterProfileIntroductionGet::SetOutput(const boost::mysql::results& result)
    {
        if (result.rows().empty())
        {
            return;
        }

        const boost::mysql::row_view& row = result.rows().at(0);
        if (row.empty())
        {
            return;
        }

        dto::ProfileIntroduction& profileIntroduction = _result.emplace();
        size_t index = 0;

        profileIntroduction.age = boost::locale::conv::from_utf(
            std::string(row.at(index++).as_string()), ServerConstant::TEXT_ENCODING);
        profileIntroduction.sex = boost::locale::conv::from_utf(
            std::string(row.at(index++).as_string()), ServerConstant::TEXT_ENCODING);
        profileIntroduction.mail = boost::locale::conv::from_utf(
            std::string(row.at(index++).as_string()), ServerConstant::TEXT_ENCODING);
        profileIntroduction.message = boost::locale::conv::from_utf(
            std::string(row.at(index++).as_string()), ServerConstant::TEXT_ENCODING);
    }
}

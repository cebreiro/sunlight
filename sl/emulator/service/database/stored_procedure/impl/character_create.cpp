#include "character_create.h"

namespace sunlight::db::sp
{
    CharacterCreate::CharacterCreate(ConnectionPool::Borrowed& conn, std::string request)
        : StoredProcedure(conn)
        , _request(std::move(request))
    {
    }

    auto CharacterCreate::GetSQL() const -> std::string_view
    {
        return "CALL character_create(?)";
    }

    auto CharacterCreate::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_request);

        return inputs;
    }

    void CharacterCreate::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

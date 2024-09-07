#include "character_delete_soft.h"

namespace sunlight::db::sp
{
    CharacterDeleteSoft::CharacterDeleteSoft(ConnectionPool::Borrowed& conn, int64_t cid)
        : StoredProcedure(conn)
        , _cid(cid)
    {
    }

    auto CharacterDeleteSoft::GetSQL() const -> std::string_view
    {
        return "CALL character_delete_soft(?)";
    }

    auto CharacterDeleteSoft::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);

        return inputs;
    }

    void CharacterDeleteSoft::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

#include "character_quest_add.h"

namespace sunlight::db::sp
{
    CharacterQuestAdd::CharacterQuestAdd(ConnectionPool::Borrowed& conn, int64_t cid, int32_t id, int32_t state,
        std::string flags, std::string data)
        : StoredProcedure(conn)
        , _cid(cid)
        , _id(id)
        , _state(state)
        , _flags(std::move(flags))
        , _data(std::move(data))
    {
    }

    auto CharacterQuestAdd::GetSQL() const -> std::string_view
    {
        return "CALL character_quest_add(?, ?, ?, ?, ?)";
    }

    auto CharacterQuestAdd::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_id);
        inputs.emplace_back(_state);
        inputs.emplace_back(_flags);
        inputs.emplace_back(_data);

        return inputs;
    }

    void CharacterQuestAdd::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

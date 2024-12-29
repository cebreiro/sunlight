#include "character_state_save.h"

namespace sunlight::db::sp
{
    CharacterStateSave::CharacterStateSave(ConnectionPool::Borrowed& conn, int64_t cid, int32_t zone, int32_t stage,
        float x, float y, float yaw, int8_t arms, int8_t running, int8_t dead, int32_t hp, int32_t sp)
        : StoredProcedure(conn)
        , _cid(cid)
        , _zone(zone)
        , _stage(stage)
        , _x(x)
        , _y(y)
        , _yaw(yaw)
        , _arms(arms)
        , _running(running)
        , _dead(dead)
        , _hp(hp)
        , _sp(sp)
    {
    }

    auto CharacterStateSave::GetSQL() const -> std::string_view
    {
        return "CALL character_state_save(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    auto CharacterStateSave::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);
        inputs.emplace_back(_zone);
        inputs.emplace_back(_stage);
        inputs.emplace_back(_x);
        inputs.emplace_back(_y);
        inputs.emplace_back(_yaw);
        inputs.emplace_back(_arms);
        inputs.emplace_back(_running);
        inputs.emplace_back(_dead);
        inputs.emplace_back(_hp);
        inputs.emplace_back(_sp);

        return inputs;
    }

    void CharacterStateSave::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}

#pragma once
#include "sl/generator/service/database/stored_procedure/stored_procedure.h"

namespace sunlight::db::sp
{
    class CharacterStateSave final : public StoredProcedure
    {
    public:
        CharacterStateSave(ConnectionPool::Borrowed& conn, int64_t cid, int32_t zone, int32_t stage,
            float x, float y, float yaw, int8_t arms, int8_t running, int8_t dead, int32_t hp, int32_t sp);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _cid = 0;
        int32_t _zone = 0;
        int32_t _stage = 0;
        float _x = 0.f;
        float _y = 0.f;
        float _yaw = 0.f;
        int8_t _arms = 0;
        int8_t _running = 0;
        int8_t _dead = 0;
        int32_t _hp = 0;
        int32_t _sp = 0;
    };
}

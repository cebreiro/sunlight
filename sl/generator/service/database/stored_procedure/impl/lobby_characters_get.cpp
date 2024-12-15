#include "lobby_characters_get.h"

#include <boost/locale.hpp>

#include "sl/generator/game/contents/item/equipment_position.h"
#include "sl/generator/server/server_constant.h"

namespace sunlight::db::sp
{
    LobbyCharactersGet::LobbyCharactersGet(ConnectionPool::Borrowed& conn, int64_t accountId, int8_t sid)
        : StoredProcedure(conn)
        , _accountId(accountId)
        , _sid(sid)
    {
    }

    auto LobbyCharactersGet::GetSQL() const -> std::string_view
    {
        return "CALL lobby_characters_get(?, ?)";
    }

    auto LobbyCharactersGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_accountId);
        inputs.emplace_back(_sid);

        return inputs;
    }

    void LobbyCharactersGet::SetOutput(const boost::mysql::results& result)
    {
        const size_t size = result.size();
        if (size < 3)
        {
            assert(false);

            return;
        }

        const boost::mysql::resultset_view& characterSet = result.at(0);
        if (characterSet.rows().empty())
        {
            return;
        }

        _results.reserve(result.size());

        for (const boost::mysql::row_view& row : characterSet.rows())
        {
            size_t index = 0;

            const int64_t characterId = row.at(index++).as_int64();
            dto::LobbyCharacter& dto = _results[characterId];

            dto.cid = characterId;
            dto.slot = static_cast<int8_t>(row.at(index++).as_int64());

            const std::string name = row.at(index++).as_string();
            dto.name = boost::locale::conv::from_utf(name, ServerConstant::TEXT_ENCODING);
            dto.level = static_cast<int32_t>(row.at(index++).as_int64());
            dto.gender = static_cast<int8_t>(row.at(index++).as_int64());
            dto.hair = static_cast<int32_t>(row.at(index++).as_int64());
            dto.hairColor = static_cast<int32_t>(row.at(index++).as_int64());
            dto.face = static_cast<int32_t>(row.at(index++).as_int64());
            dto.skinColor = static_cast<int32_t>(row.at(index++).as_int64());
            dto.zone = static_cast<int32_t>(row.at(index++).as_int64());
            dto.arms = static_cast<int8_t>(row.at(index++).as_int64());
        }

        const boost::mysql::resultset_view& itemSet = result.at(1);

        for (const boost::mysql::row_view& row : itemSet.rows())
        {
            size_t index = 0;

            const int64_t cid = row.at(index++).as_int64();
            const int32_t itemDataId = static_cast<int32_t>(row.at(index++).as_int64());
            const int32_t position = static_cast<int32_t>(row.at(index++).as_int64());

            auto iter = _results.find(cid);
            if (iter == _results.end())
            {
                assert(false);

                continue;
            }

            switch (static_cast<EquipmentPosition>(position))
            {
            case EquipmentPosition::Hat:
                iter->second.hatItemId = itemDataId;
                break;
            case EquipmentPosition::Jacket:
                iter->second.jacketItemId = itemDataId;
                break;
            case EquipmentPosition::Gloves:
                iter->second.glovesItemId = itemDataId;
                break;
            case EquipmentPosition::Pants:
                iter->second.pantsItemId = itemDataId;
                break;
            case EquipmentPosition::Shoes:
                iter->second.shoesItemId = itemDataId;
                break;
            default:
                assert(false);

                SUNLIGHT_APP_LOG_CRITICAL(fmt::format("[{}] invalid equipment position. aid: {}, cid: {}, pos: {}",
                    __FUNCTION__, _accountId, cid, position));
            }
        }

        const boost::mysql::resultset_view& jobSet = result.at(2);

        for (const boost::mysql::row_view& row : jobSet.rows())
        {
            size_t index = 0;

            const int64_t cid = row.at(index++).as_int64();
            const int32_t job = static_cast<int32_t>(row.at(index++).as_int64());
            const int32_t type = static_cast<int32_t>(row.at(index++).as_int64());

            auto iter = _results.find(cid);
            if (iter == _results.end())
            {
                assert(false);

                continue;
            }

            if (type == 0)
            {
                iter->second.job1 = job;
            }
            else if (type == 1)
            {
                iter->second.job2 = job;
            }
            else
            {
                assert(false);

                SUNLIGHT_APP_LOG_CRITICAL(fmt::format("[{}] invalid job type. aid: {}, cid: {}, type: {}",
                    __FUNCTION__, _accountId, cid, type));
            }
        }
    }
}
